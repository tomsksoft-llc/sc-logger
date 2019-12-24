/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <memory>
#include <fstream>
#include <mutex>
#include <map>
#include <set>

#include <scl/options.h>
#include <scl/levels.h>
#include <scl/result.h>
#include <scf/detail/type_matching.h>

namespace scl {

class Logger {
public:
    /**
     * Init a Logger singleton instance.
     * Use the MakeOptions() method to make it easier.
     */
    static InitResult Init(const Options &options);

    static void Uninit();

    static void Record(Level level, const std::string &message);

    static void SesRecord(Level level,
                          const std::string &session_id,
                          const std::string &message);

    template<typename ActT>
    inline static void ActRecord(Level level,
                                 const ActT &action,
                                 const std::string &message) {
        AssertInstance();

        const auto session_id = std::nullopt;
        Instance->RecordImpl(level, session_id, ActionAsString(action), message);
    }

    template<typename ActT>
    static void SesActRecord(Level level,
                             const std::string &session_id,
                             const ActT &action,
                             const std::string &message) {
        AssertInstance();
        Instance->RecordImpl(level, session_id, ActionAsString(action), message);
    }

private:
    inline static void AssertInstance() {
        if (!Instance) {
            throw std::runtime_error("Logger instance is not initialized yet");
        }
    }

    /**
     * Container of specifiers  within a template name
     */
    using SpecifierPositions = std::map<char /*specifier*/, std::size_t /*position*/>;

    /**
     * Container of specifiers
     */
    using SpecifierSet = std::set<char /*specifier*/>;

    enum class CheckFileSizeResult : int {
        NotExists = 0,
        Allowed,
        IsOverflowed,
    };

    enum class OpenFileResult : int {
        Ok = 0,
        CantOpenFile,
    };

    enum class TemplateError : int {
        Ok = 0,
        DuplicateSpecifiers,
        UnknownSpecifiers,
    };

    template<typename ActT>
    static std::string ActionAsString(const ActT &action) {
        constexpr bool is_there_to_string = scf::detail::IsThereToStringFor<ActT>::value;
        constexpr bool is_string = scf::detail::IsString(action);
        static_assert(is_there_to_string || is_string,
                      "there must be a ToString() function for the action");

        const auto session_id = std::nullopt;
        if constexpr (is_there_to_string) {
            return ToString(action);
        } else {
            return action;
        }
    }

    /**
     * Get positions of the specifiers (from the search_specifiers).
     * Note: template should contain only non-repeating specifiers from the search_specifiers container.
     * @param templ - source template
     * @param search_specifiers - container of specifiers that we have to find within the template
     * @param error - output value that will be set within the function (Ok if there was no error, else error code)
     * @return - positions of the specifiers
     */
    static SpecifierPositions BreakDownTemplate(std::string_view templ,
                                                const SpecifierSet &search_specifiers,
                                                TemplateError &error);

    static std::string CurTimeStr();

    /**
     * Private constructor.
     * @param options - logger options.
     */
    explicit Logger(Options options);

    /**
     * Private constructor.
     * @param options - logger options.
     */
    explicit Logger(Options options, SpecifierPositions &&file_name_specifiers);

    void RecordImpl(Level level,
                    const std::optional<std::string> &session_id,
                    const std::optional<std::string> &action,
                    const std::string &message);

    /**
     * Try to open the log file at the specified path with the name corresponding to the specified template.
     * Note: the method should be called before the mutex will be locked.
     * @return - true if the file has been opened successfully, else false.
     */
    OpenFileResult OpenFile();

    /**
     * Check if the log file is not overflowed (file size < options.size_limit).
     * Note: if the options.size_limit is not set, then the method will return CheckFileSizeResult::Ok always.
     * @param file - path to the file whose size should be checked.
     * @param record_data_size - size of recorded data. The argument should be put if the method is called on new record data
     *                           in order to check that the size of existing file + record_data_size is no more than size_limit
     * @return - Ok if the writing to the file is allowed, else IsOverflowed.
     */
    CheckFileSizeResult CheckFileSize(const fs::path &file, std::size_t record_data_size = 0) const;

    /**
     * Compile full path to the log file (directory + filled file name template)
     */
    fs::path CompileFullPath() const;

    /**
     * Compile full record with the following format:
     * time | parent_pid | current_pid | session_id | message
     * where session_id may not be (depends on m_options.session_id)
     * @param record
     * @return
     */
    std::string CompileRecord(const RecordInfo &record) const;

    /**
     * If the SCL_MULTITHREADED is defined, lock the m_mutex, else do nothing
     * @return mutex guard or std::nullopt
     */
    std::optional<std::lock_guard<std::mutex>> LockMutex();

    /**
     * Singleton logger instance.
     */
    static std::unique_ptr<Logger> Instance;

    /**
     * Current rotation iteration. The value increase on any the log file opening.
     */
    std::size_t m_rotation_iteration = 0;

    /**
     * Logger options.
     */
    Options m_options;

    /**
     * Log file stream (only writing).
     */
    std::ofstream m_log_file;

    /**
     * The full path to the opened log file.
     */
    fs::path m_log_file_path;

    SpecifierPositions m_file_name_specifiers;

#ifdef SCL_MULTITHREADED
    std::mutex m_record_mutex;
#endif
};

} // end of scl
