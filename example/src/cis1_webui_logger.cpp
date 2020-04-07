#include <cassert>
#include <thread>
#include <cis1_webui_logger/webui_logger.h>
#include <scf/scf.h>
#include <scl/console_recorder.h>
#include <scl/file_recorder.h>

cis1::webui_logger::LoggerPtr LoggerInstance;

#define LOG(level, format, ...) \
LoggerInstance->Record(level, SCFormat(format, ##__VA_ARGS__))

#define EXLOG(level, protocol, format, ...) \
LoggerInstance->ExRecord(level, protocol, __FUNCTION__, context.addr, context.email, SCFormat(format, ##__VA_ARGS__))

struct UserType {
    std::string data;
};

std::string ToString(const UserType &val) {
    return "{" + val.data + "}";
}

template<typename Value, typename Error>
auto &&Unwrap(std::variant<Value, Error> &&result) {
    const auto *error = std::get_if<Error>(&result);
    assert(!error);
    return std::get<Value>(std::move(result));
}

struct UserContext {
    std::string email;
    std::string addr;
};

using Level = scl::Level;

using Protocol = cis1::webui_logger::Protocol;

void handler_one(const UserContext &context) {
    EXLOG(Level::Debug, Protocol::HTTP_POST, "Debug %s", "message");
    EXLOG(Level::Info, Protocol::HTTP_POST, "Info message (double = %f, char = '%c')", 3.14, '@');
}

void handler_two(const UserContext &context) {
    EXLOG(Level::Debug, Protocol::WS, "Debug %s", "message");
    EXLOG(Level::Info, Protocol::WS, "Info message (double = %f, char = '%c')", 3.14, '@');
}

using WebuiLogger = cis1::webui_logger::WebuiLogger;
using WebuiRecord = cis1::webui_logger::WebuiRecord;

int main(int argc, char *argv[]) {
    const bool align = true;

    cis1::webui_logger::WebuiLogger::Options options{Level::Debug};

    scl::ConsoleRecorder<WebuiRecord>::Options console_options{align};

    scl::FileRecorder<WebuiRecord>::Options file_options;
    file_options.log_directory = std::filesystem::current_path();
    file_options.file_name_template = "cis1_webui_log.%t.%n.txt";
    file_options.size_limit = 600;
    file_options.align = align;

    scl::RecordersCont<WebuiRecord> recorders;
    recorders.push_back(scl::ConsoleRecorder<WebuiRecord>::Init(console_options));
    recorders.push_back(Unwrap(scl::FileRecorder<WebuiRecord>::Init(file_options)));

    LoggerInstance = Unwrap(cis1::webui_logger::WebuiLogger::Init(options, std::move(recorders)));

    UserType val{"12345"};
    UserContext context{"first_usr@example.com", "127.0.0.1:8000"};

    LOG(Level::Action, "Run handler_one() (UserType = %U)", val);
    handler_one(context);
    LOG(Level::Action, "Stop handler_one()");

    context.email = "second_usr@exampl.com";
    context.addr = "127.0.0.1:8001";

    LOG(Level::Action, "Run handler_two() (double = %f, char = '%c')", 3.14, '@');
    handler_two(context);
    LOG(Level::Action, "Stop handler_two()");
}
