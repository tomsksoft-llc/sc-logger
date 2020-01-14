#include <cassert>
#include <thread>
#include <scl/logger.h>
#include <scf/scf.h>
#include <scl/console_recorder.h>
#include <scl/file_recorder.h>

std::unique_ptr<scl::Logger> LoggerInstance;

#define LOG(level, format, ...) LoggerInstance->Record(level, SCFormat(format, ##__VA_ARGS__))
#define LOG_S(level, session_id, format, ...) LoggerInstance->SesRecord(level, session_id, SCFormat(format, ##__VA_ARGS__))
#define LOG_A(level, action_id, format, ...) LoggerInstance->ActRecord(level, action_id, SCFormat(format, ##__VA_ARGS__))
#define LOG_SA(level, session_id, action_id, format, ...) LoggerInstance->SesActRecord(level, session_id, action_id, SCFormat(format, ##__VA_ARGS__))


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
    file_options.log_directory = std::filesystem::current_path();
    file_options.file_name_template = "file.%t.%n.txt";
    file_options.size_limit = 300;
    file_options.align_info = align_info;

    scl::RecordersCont recorders;
    recorders.push_back(scl::ConsoleRecorder::Init(console_options));
    recorders.push_back(Unwrap(scl::FileRecorder::Init(file_options)));

    LoggerInstance = Unwrap(scl::Logger::Init(options, std::move(recorders)));

    UserType val{"12345"};
    LOG(Level::Debug, "foo %s", "bar");
    LOG(Level::Debug, "Action message (UserType = %U)", val);
    LOG_S(Level::Error, "some session identifier", "Error message");
    LOG_A(Level::Info, UserActions::Act2, "Info message");
    LOG_SA(Level::Action,
           "some session identifier",
           "some action as string",
           "Action message (double = %f, char = '%c')", 3.14, '@');
}
