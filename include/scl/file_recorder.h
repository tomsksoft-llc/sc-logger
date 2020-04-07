/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <fstream>
#include <map>
#include <set>
#include <mutex>
#include <filesystem>
#include <variant>

#include <scl/levels.h>
#include <scl/recorder.h>
#include <scl/record.h>
#include <scf/detail/type_matching.h>
#include <scl/detail/misc.h>

namespace scl {

template<typename RecordT>
class FileRecorder;

namespace fs = std::filesystem;

/**
 * Non-moving file recorder pointer alias.
 */
template<typename RecordT>
using FileRecorderPtr = std::unique_ptr<FileRecorder<RecordT>>;

/**
 * File recorder that implement the IRecorder interface.
 * The recorder is used to write records to a file
 * and allows automatic rotation (by substituting values instead of filename template specifiers).
 */
template<typename RecordT>
class FileRecorder : public IRecorder<RecordT> {
public:
    /**
     * Initialization error info.
     */
    enum class InitError {
        PathNotExists = 1,
        PathIsNotDirectory,
        IncorrectFileNameTemplate,
        CantOpenFile,
    };

    /**
     * Initialization result: ether pointer to an initialized file recorder or an error info.
     */
    using InitResult = std::variant<FileRecorderPtr<RecordT>, InitError>;

    /**
     * File recorder options.
     */
    struct Options {
        /**
         * Path to a log directory.
         */
        fs::path log_directory;

        /**
         * File name template. The template is used to allow a log rotation.
         * The file_name_template value may contain the following specifiers:
         *   %t - current time (the specifier will be replaced by the filled "%Y-%m-%d-%H-%M-%S" template);
         *   %n - number of rotation iteration (start with '1').
         *
         * If the size_limit value is set (!= std::nullopt),
         * the file_name_template value must contain at least one of the above specifiers.
         */
        std::string file_name_template;

        /**
         * Allow a log rotation, when a count of log file lines reaches a particular limit.
         */
        std::optional<std::size_t> size_limit = std::nullopt;

        /**
         * Allow to align entry attributes, if the values is true.
         */
        bool align = false;
    };

    static std::string ToStr(InitError err) {
        switch (err) {
            case InitError::PathNotExists:
                return "PathNotExists";
            case InitError::PathIsNotDirectory:
                return "PathIsNotDirectory";
            case InitError::IncorrectFileNameTemplate:
                return "IncorrectFileNameTemplate";
            case InitError::CantOpenFile:
                return "CantOpenFile";
            default:
                return "Unknown";
        }
    }

    /**
     * Init a FileRecorder instance.
     * @param options - file recorder options
     * @return - ether pointer to an initialized recorder or an error info
     */
    static InitResult Init(const Options &options) {
        using Error = InitError;

        const auto &log_directory = options.log_directory;
        const auto &file_name_template = options.file_name_template;

        if (log_directory.empty() || !fs::exists(log_directory)) {
            return Error::PathNotExists;
        }

        if (!fs::is_directory(log_directory)) {
            return Error::PathIsNotDirectory;
        }

        if (file_name_template.empty()) {
            return Error::IncorrectFileNameTemplate;
        }

        auto &&[error, specifier_positions, specifiers]
        = ProcessTemplate(file_name_template,
                          {
                              detail::file_name_formatting::current_time_spc_k,
                              detail::file_name_formatting::rotation_iteration_number_spc_k
                          });

        if (error != TemplateError::Ok) {
            return Error::IncorrectFileNameTemplate;
        }

        std::unique_ptr<FileRecorder<RecordT>> instance;
        instance.reset(new FileRecorder<RecordT>(options,
                                                 std::move(specifier_positions),
                                                 std::move(specifiers)));

        // there is no need to lock a mutex,
        // because the function should be called once on an application start
        const auto open_file_result = instance->OpenFile();
        if (open_file_result != OpenFileResult::Ok) {
            return Error::CantOpenFile;
        }

        return instance;
    }

    /**
     * Default derived dtor.
     */
    ~FileRecorder() final = default;

    /**
     * @overload
     */
    void OnRecord(const RecordT &record) final {
        // if last time we couldn't open a file, don't try to do it again
        if (!m_log_file.is_open()) {
            // there is no need to check, open and write to file
            return;
        }

        // a log file is opened already

        const auto record_str
            = m_options.align
              ? record.ToAlignedString()
              : record.ToString();

        // lock the mutex here, before the OpenFile() will be called
        const auto lock = LockMutex();
        const CheckFileSizeResult size_result = CheckFileSize(m_log_file_path, record_str.size());
        if (size_result != CheckFileSizeResult::Allowed
            // the file could be deleted or overflowed, try to open file again
            && OpenFile() != OpenFileResult::Ok
            ) {
            // couldn't open a log file
            return;
        }

        m_log_file << record_str << std::endl;
    }

private:
    /**
     * Container of specifiers within a template name ordered by positions.
     */
    using SpecifierPositions = std::map<std::size_t /*position*/, char /*specifier*/>;

    /**
     * Container of specifiers.
     */
    using SpecifierSet = std::set<char /*specifier*/>;

    /**
     * Enumeration of the CheckFileSize() method results.
     */
    enum class CheckFileSizeResult : int {
        NotExists = 0,
        Allowed,
        IsOverflowed,
    };

    /**
     * Enumeration of the OpenFile() method results.
     */
    enum class OpenFileResult : int {
        Ok = 0,
        CantOpenFile,
    };

    /**
     * Enumeration of the possible filename template errors.
     */
    enum class TemplateError : int {
        Ok = 0,
        DuplicateSpecifiers,
        UnknownSpecifiers,
    };

    /**
     * Returning values of the ProcessTemplate() method.
     */
    struct ProcessTemplateResult {
        /**
         * Status of the filename template processing.
         * Ok if the template was processed correctly, else corresponding error.
         */
        TemplateError error = TemplateError::Ok;

        /**
         * Specifiers and corresponding positions that are in a filename template.
         */
        SpecifierPositions file_name_specifier_positions;

        /**
         * Set of specifiers that are in a filename template.
         */
        SpecifierSet file_name_specifiers;
    };

    /**
     * Get positions of the specifiers (from the search_specifiers).
     * Note: template should contain only non-repeating specifiers from the search_specifiers container.
     * @param templ - source template
     * @param search_specifiers - container of specifiers that we have to find within the template
     * @return - set of values (see the ProcessTemplateResult structure)
     */
    static ProcessTemplateResult ProcessTemplate(std::string_view templ,
                                                 const SpecifierSet &search_specifiers) {
        using Error = TemplateError;
        ProcessTemplateResult result;

        const auto find_specifier_pos_fn
            = [&templ, &search_specifiers](std::size_t start_pos) -> std::size_t {
                // do not pass the pos value by reference, the function should new value
                const auto pos = templ.find(detail::file_name_formatting::start_of_spec_subseq_k, start_pos);
                if (pos == std::string::npos) {
                    return std::string::npos;
                }

                return pos + 1 >= templ.size()
                       // the '%' is the last character of the file_name string
                       // therefore there is no need to replace the character
                       ? std::string::npos
                       // else return position to the '%' character
                       : pos;
            };

        SpecifierPositions specifier_positions;
        SpecifierSet specifiers;
        char specifier = 0;
        std::size_t pos = 0;

        while ((pos = find_specifier_pos_fn(pos)) != std::string::npos) {
            specifier = templ[pos + 1];
            if (!search_specifiers.count(specifier)) {
                result.error = Error::UnknownSpecifiers;
                return result;
            }

            if (specifiers.count(specifier)) {
                result.error = Error::DuplicateSpecifiers;
                return result;
            }

            specifier_positions[pos] = specifier;
            specifiers.insert(specifier);
            ++pos;
        }

        result.file_name_specifier_positions = std::move(specifier_positions);
        result.file_name_specifiers = std::move(specifiers);
        return result;
    }

    /**
     * Private ctor.
     * @param options - file recorder options
     * @param file_name_specifier_positions - specifiers and corresponding positions
     *                                        that are in a filename template
     * @param file_name_specifiers - set of specifiers that are in a filename template
     */
    explicit FileRecorder(Options options,
                          SpecifierPositions &&file_name_specifier_positions,
                          SpecifierSet &&file_name_specifiers)
        : m_options(std::move(options)),
          m_file_name_specifier_positions(std::move(file_name_specifier_positions)),
          m_file_name_specifiers(std::move(file_name_specifiers)) {
    }

    /**
     * Try to open the log file at the specified path with the name corresponding to the specified template.
     * Note: the method should be called before the mutex will be locked.
     * @return - true if the file has been opened successfully, else false.
     */
    OpenFileResult OpenFile() {
        using Result = OpenFileResult;
        // if the method was called, the path_info should be set
        fs::path log_file_path;

        // true if the file name template contains a '%n' specifier
        const bool file_name_can_be_rotated
            = static_cast<bool>(m_file_name_specifiers.count(
                detail::file_name_formatting::rotation_iteration_number_spc_k));

        // true if the file name template contains a '%t' specifier
        const bool file_name_contains_time_specifier
            = static_cast<bool>(m_file_name_specifiers.count(
                detail::file_name_formatting::current_time_spc_k));

        const auto current_time = std::time(nullptr);
        // check that the last rotation time was changed and we can
        if (file_name_contains_time_specifier && current_time != m_last_file_open_time) {
            m_rotation_iteration = 0;
        }

        m_last_file_open_time = current_time;

        m_log_file.close();
        m_log_file_path.clear();

        CheckFileSizeResult file_size_result;
        do {
            // number of rotation iteration should start with '1'
            ++m_rotation_iteration;

            log_file_path = CompileFullPath();
            file_size_result = CheckFileSize(log_file_path);
        } while (file_size_result == CheckFileSizeResult::IsOverflowed
                 && file_name_can_be_rotated);

        if (file_size_result == CheckFileSizeResult::IsOverflowed) {
            // file name template doesn't contain the "%n" specifier,
            // therefore we cannot change a file name by increasing the m_rotation_iteration
            return Result::CantOpenFile;
        }

        m_log_file.open(log_file_path, std::ios::app);
        if (!m_log_file.is_open()) {
            return Result::CantOpenFile;
        }

        m_log_file_path = log_file_path;
        return Result::Ok;
    }

    /**
     * Check a file size.
     * If the record_data_size is not 0,
     * then the file's size should be less then options.size_limit - record_data_size.
     * @param file - path to a file
     * @param record_data_size - optional record data size
     * @return - one of the CheckFileSizeResult values
     */
    CheckFileSizeResult CheckFileSize(const fs::path &file,
                                      std::size_t record_data_size = 0) const {
        using Result = CheckFileSizeResult;

        std::error_code ec{};
        const auto size = fs::file_size(file, ec);
        if (ec) {
            // the file could be deleted or not created
            return Result::NotExists;
        }

        if (!m_options.size_limit) {
            // there is no need to rotate the log file
            return Result::Allowed;
        }

        const auto size_limit = *m_options.size_limit;
        if (size_limit <= size + record_data_size) {
            return Result::IsOverflowed;
        }

        return Result::Allowed;
    }

    /**
     * Compile full path using: file name template, rotation number and path to a log directory.
     * @return - full path
     */
    fs::path CompileFullPath() const {
        std::string file_name = m_options.file_name_template;

        std::size_t offset = 0;
        std::string dest_str;
        for (auto[pos, specifier] : m_file_name_specifier_positions) {
            if (specifier == detail::file_name_formatting::current_time_spc_k) {
                dest_str = CurTimeStr();
            } else if (specifier == detail::file_name_formatting::rotation_iteration_number_spc_k) {
                dest_str = std::to_string(m_rotation_iteration);
            } else {
                // something went wrong, please fix that
                throw std::runtime_error("internal error");
            }

            file_name.replace(pos + offset, detail::file_name_formatting::specifier_size, dest_str);
            offset += dest_str.size() - detail::file_name_formatting::specifier_size;
        }

        return m_options.log_directory / file_name;
    }

    /**
     * If the SCL_MULTITHREADED is defined, lock the m_mutex, else do nothing.
     * @return mutex guard or std::nullopt
     */
    std::optional<std::lock_guard<std::mutex>> LockMutex() {
#ifdef SCL_MULTITHREADED
        return std::make_optional<std::lock_guard<std::mutex>>(m_record_mutex);
#else
        return std::nullopt;
#endif
    }

    /**
     * File recorder's options.
     */
    Options m_options;

    /**
     * Specifiers and corresponding positions that are in filename template.
     */
    SpecifierPositions m_file_name_specifier_positions;

    /**
     * Set of specifiers that are in filename template.
     */
    SpecifierSet m_file_name_specifiers;

    /**
     * Log file stream (only writing).
     */
    std::ofstream m_log_file;

    /**
     * The full path to the opened log file.
     */
    fs::path m_log_file_path;

    /**
     * Current rotation iteration. The value increases each time the log file is opened.
     */
    std::size_t m_rotation_iteration = 0;

    /**
     * Last file open time. The value changes each time the log file is opened.
     */
    std::time_t m_last_file_open_time = 0;

#ifdef SCL_MULTITHREADED
    std::mutex m_record_mutex;
#endif
};

} // end of scl::detail
