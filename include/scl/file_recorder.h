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

namespace scl {

class FileRecorder;

namespace fs = std::filesystem;

/**
 * Non-moving file recorder pointer alias.
 */
using FileRecorderPtr = std::unique_ptr<FileRecorder>;

/**
 * File recorder that implement the IRecorder interface.
 * The recorder is used to write records to a file
 * and allows automatic rotation (by substituting values instead of filename template specifiers).
 */
class FileRecorder : public IRecorder {
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
    using InitResult = std::variant<FileRecorderPtr, InitError>;

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
    static InitResult Init(const Options &options);

    /**
     * Default derived dtor.
     */
    ~FileRecorder() final = default;

    /**
     * @overload
     */
    void OnRecord(const IRecord &record) final;

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
                                                 const SpecifierSet &search_specifiers);

    /**
     * Private ctor.
     * @param options - file recorder options
     * @param file_name_specifier_positions - specifiers and corresponding positions
     *                                        that are in a filename template
     * @param file_name_specifiers - set of specifiers that are in a filename template
     */
    explicit FileRecorder(Options options,
                          SpecifierPositions &&file_name_specifier_positions,
                          SpecifierSet &&file_name_specifiers);

    /**
     * Try to open the log file at the specified path with the name corresponding to the specified template.
     * Note: the method should be called before the mutex will be locked.
     * @return - true if the file has been opened successfully, else false.
     */
    OpenFileResult OpenFile();

    /**
     * Check a file size.
     * If the record_data_size is not 0,
     * then the file's size should be less then options.size_limit - record_data_size.
     * @param file - path to a file
     * @param record_data_size - optional record data size
     * @return - one of the CheckFileSizeResult values
     */
    CheckFileSizeResult CheckFileSize(const fs::path &file,
                                      std::size_t record_data_size = 0) const;

    /**
     * Compile full path using: file name template, rotation number and path to a log directory.
     * @return - full path
     */
    fs::path CompileFullPath() const;

    /**
     * If the SCL_MULTITHREADED is defined, lock the m_mutex, else do nothing.
     * @return mutex guard or std::nullopt
     */
    std::optional<std::lock_guard<std::mutex>> LockMutex();

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
