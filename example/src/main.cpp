#include <cassert>
#include <thread>
#include <scl/logger.h>
#include <scf/scf.h>
#include <scl/console_recorder.h>
#include <scl/file_recorder.h>

std::unique_ptr<scl::Logger> ConsoleLogger;
std::unique_ptr<scl::Logger> FileLogger;

#define LOG(logger, level, format, ...) logger->Record(level, SCFormat(format, ##__VA_ARGS__))
#define LOG_S(logger, level, session_id, format, ...) logger->SesRecord(level, session_id, SCFormat(format, ##__VA_ARGS__))
#define LOG_A(logger, level, action_id, format, ...) logger->ActRecord(level, action_id, SCFormat(format, ##__VA_ARGS__))
#define LOG_SA(logger, level, session_id, action_id, format, ...) logger->SesActRecord(level, session_id, action_id, SCFormat(format, ##__VA_ARGS__))

#define TEE_LOG(level, format, ...) \
LOG(ConsoleLogger, level, format, ##__VA_ARGS__); \
LOG(FileLogger, level, format, ##__VA_ARGS__) \

#define TEE_LOG_S(level, session_id, format, ...) \
LOG_S(ConsoleLogger, level, session_id, format, ##__VA_ARGS__); \
LOG_S(FileLogger, level, session_id, format, ##__VA_ARGS__)

#define TEE_LOG_A(level, action_id, format, ...) \
LOG_A(ConsoleLogger, level, action_id, format, ##__VA_ARGS__); \
LOG_A(FileLogger, level, action_id, format, ##__VA_ARGS__)

#define TEE_LOG_SA(level, session_id, action_id, format, ...) \
LOG_SA(ConsoleLogger, level, session_id, action_id, format, ##__VA_ARGS__); \
LOG_SA(FileLogger, level, session_id, action_id, format, ##__VA_ARGS__)

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

int main(int argc, char *argv[]) {
    using Level = scl::Level;

    const auto align_info = scl::AlignInfo{10, 20};

    scl::Logger::Options options{Level::Debug, 1};
    scl::ConsoleRecorder::Options console_options{align_info};
    scl::FileRecorder::Options file_options;
    file_options.log_directory = "/home/bso/tmp";
    file_options.file_name_template = "file.%t.%n.txt";
    file_options.size_limit = 1000;
    file_options.align_info = align_info;

    auto console_recorder = scl::ConsoleRecorder::Init(console_options);
    auto file_recorder = Unwrap(scl::FileRecorder::Init(file_options));

    ConsoleLogger = Unwrap(scl::Logger::Init(options, std::move(console_recorder)));
    FileLogger = Unwrap(scl::Logger::Init(options, std::move(file_recorder)));

    UserType val{"12345"};
    TEE_LOG(Level::Debug, "foo %s", "bar");
    TEE_LOG(Level::Debug, "Action message (UserType = %U)", val);
    TEE_LOG_S(Level::Error, "some session identifier", "Error message");
    TEE_LOG_A(Level::Info, UserActions::Act2, "Info message");
    TEE_LOG_SA(Level::Action,
               "some session identifier",
               "some action as string",
               "Action message (double = %f, char = '%c')", 3.14, '@');
}

