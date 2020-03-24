#include <cassert>
#include <thread>
#include <cis1_core_logger/core_logger.h>
#include <scf/scf.h>
#include <scl/console_recorder.h>
#include <scl/file_recorder.h>

cis1::core_logger::LoggerPtr LoggerInstance;

#define LOG(level, format, ...) LoggerInstance->Record(level, SCFormat(format, ##__VA_ARGS__))
#define LOG_S(level, format, ...) LoggerInstance->SesRecord(level, SCFormat(format, ##__VA_ARGS__))
#define LOG_A(level, action_id, format, ...) LoggerInstance->ActRecord(level, action_id, SCFormat(format, ##__VA_ARGS__))
#define LOG_SA(level, action_id, format, ...) LoggerInstance->SesActRecord(level, action_id, SCFormat(format, ##__VA_ARGS__))


struct UserType {
    std::string data;
};

std::string ToString(const UserType &val) {
    return "{" + val.data + "}";
}

enum UserActions : unsigned int {
    Act1 = 0,
    Act2
};

std::string ToString(UserActions action) {
    if (action == UserActions::Act1) {
        return "Act1";
    } else if (action == UserActions::Act2) {
        return "Act2";
    }

    return "unknown";
}

template<typename Value, typename Error>
auto &&Unwrap(std::variant<Value, Error> &&result) {
    const auto *error = std::get_if<Error>(&result);
    assert(!error);
    return std::get<Value>(std::move(result));
}

using CoreLogger = cis1::core_logger::CoreLogger;
using CoreRecord = cis1::core_logger::CoreRecord;

int main(int argc, char *argv[]) {
    using Level = scl::Level;

    const scl::ProcessId pid = 12345;
    const scl::ProcessId ppid = 1;
    const bool align = true;

    cis1::core_logger::CoreLogger::Options options{Level::Debug, pid, ppid, "some session id"};

    scl::ConsoleRecorder<CoreRecord>::Options console_options{align};

    scl::FileRecorder<CoreRecord>::Options file_options;
    file_options.log_directory = std::filesystem::current_path();
    file_options.file_name_template = "cis1_core_log.%t.%n.txt";
    file_options.size_limit = 300;
    file_options.align = align;

    scl::RecordersCont<CoreRecord> recorders;
    recorders.push_back(scl::ConsoleRecorder<CoreRecord>::Init(console_options));
    recorders.push_back(Unwrap(scl::FileRecorder<CoreRecord>::Init(file_options)));

    LoggerInstance = Unwrap(cis1::core_logger::CoreLogger::Init(options, std::move(recorders)));

    UserType val{"12345"};
    LOG(Level::Debug, "Debug %s", "message");
    LOG_S(Level::Error, "Error message (UserType = %U)", val);
    LOG_A(Level::Info, UserActions::Act2, "Info message");
    LOG_SA(Level::Action,
           "some str action",
           "Action message (double = %f, char = '%c')", 3.14, '@');
}
