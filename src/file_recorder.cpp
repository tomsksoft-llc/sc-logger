#include <scl/file_recorder.h>
#include <scl/detail/misc.h>

namespace scl {

FileRecorder::InitResult FileRecorder::Init(const Options &options) {
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

    std::unique_ptr<FileRecorder> instance;
    instance.reset(new FileRecorder(options,
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

FileRecorder::ProcessTemplateResult FileRecorder::ProcessTemplate(std::string_view templ,
                                                                  const FileRecorder::SpecifierSet &search_specifiers) {
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

void FileRecorder::OnRecord(const RecordInfo &record) {
    // if last time we couldn't open a file, don't try to do it again
    if (!m_log_file.is_open()) {
        // there is no need to check, open and write to file
        return;
    }

    // a log file is opened already

    const auto record_str
        = m_options.align_info
          ? ToString(record, *m_options.align_info)
          : ToString(record);

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

FileRecorder::FileRecorder(Options options,
                           SpecifierPositions &&file_name_specifier_positions,
                           SpecifierSet &&file_name_specifiers)
    : m_options(std::move(options)),
      m_file_name_specifier_positions(std::move(file_name_specifier_positions)),
      m_file_name_specifiers(std::move(file_name_specifiers)) {
}

FileRecorder::OpenFileResult FileRecorder::OpenFile() {
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

FileRecorder::CheckFileSizeResult FileRecorder::CheckFileSize(const fs::path &file,
                                                              std::size_t record_data_size) const {
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

fs::path FileRecorder::CompileFullPath() const {
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

std::optional<std::lock_guard<std::mutex>> FileRecorder::LockMutex() {
#ifdef SCL_MULTITHREADED
    return std::make_optional<std::lock_guard<std::mutex>>(m_record_mutex);
#else
    return std::nullopt;
#endif
}

}
