#include <cassert>
#include <thread>
#include <scl/logger.h>
#include <scf/scf.h>
#include <scl/session_logger.h>

#define LOG(level, format, ...) \
scl::Logger::Record(level, SCFormat(format, ##__VA_ARGS__))

#define SES_LOG(level, session, format, ...) \
scl::Logger::SesRecord(level, session, SCFormat(format, ##__VA_ARGS__))

#define ACT_LOG(level, action, format, ...) \
scl::Logger::ActRecord(level, action, SCFormat(format, ##__VA_ARGS__))

#define SES_ACT_LOG(level, session, action, format, ...) \
scl::Logger::SesActRecord(level, session, action, SCFormat(format, ##__VA_ARGS__))


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

int main(int argc, char *argv[]) {
//    using Level = scl::Level;
//
//    scl::Options options;
//    scl::fs::current_path();
//    options.path_info = scl::PathInfo("/home/bso/tmp", "file.%n.txt");
//    options.parent_pid = 1;
//    options.level = scl::Level::Debug;
//    options.use_console_output = true;
//    options.exit_on_cant_open_file = false;
//    options.size_limit = 1000;
//    options.align_info = scl::AlignInfo(10, 20);
//    const scl::InitResult result = scl::Logger::Init(options);
//    assert(result == scl::InitResult::Ok);
//
//    LOG(Level::Debug, "foo %s", "bar");
//
//    UserType val{"12345"};
//    LOG(Level::Debug, "Action message (UserType = %U)", val);
//    SES_LOG(Level::Error, "some session identifier", "Error message");
//    ACT_LOG(Level::Info, UserActions::Act2, "Info message");
//    SES_ACT_LOG(Level::Action,
//                "some session identifier",
//                "some action as string",
//                "Action message (double = %f, char = '%c')", 3.14, '@');
//
//    scl::Logger::Uninit();
//    return 0;
}

