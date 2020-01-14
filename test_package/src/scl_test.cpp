#include <gtest/gtest.h>
#include <scl/logger.h>
#include <scl/console_recorder.h>
#include <scl/file_recorder.h>

#define EXPECT_ERROR(result, error) \
{ const auto stor_err = std::get_if<decltype(error)>(&result); ASSERT_TRUE(stor_err && *stor_err == error); }

template<typename Value, typename Error>
void Unwrap(Value &dst, std::variant<Value, Error> &&src) {
    const auto *val = std::get_if<Value>(&src);
    ASSERT_TRUE(val != nullptr);
    dst = std::get<Value>(std::move(src));
}


using namespace scl;

TEST(SclTest, LoggerIncorrectLogLevelError) {
    using Error = Logger::InitError;

    Logger::Options options{static_cast<Level>(50)};
    RecordersCont cont;

    auto result = Logger::Init(options, std::move(cont));
    EXPECT_ERROR(result, Error::IncorrectLogLevel);
}

TEST(SclTest, LoggerNoRecordersError) {
    using Error = Logger::InitError;

    Logger::Options options{};
    RecordersCont cont;

    auto result = Logger::Init(options, std::move(cont));
    EXPECT_ERROR(result, Error::NoRecorders);
}

TEST(SclTest, LoggerUnallocatedRecorderError) {
    using Error = Logger::InitError;

    Logger::Options options{};
    RecordersCont cont;
    // push an unallocated recorder to the cont
    cont.push_back(std::unique_ptr<IRecorder>());

    auto result = Logger::Init(options, std::move(cont));
    EXPECT_ERROR(result, Error::UnallocatedRecorder);
}

TEST(SclTest, LoggerInitialized) {
    using Error = Logger::InitError;

    Logger::Options options{Level::Debug};
    ConsoleRecorder::Options console_recorder_options{};

    RecordersCont cont;
    // push an unallocated recorder to the cont
    cont.push_back(ConsoleRecorder::Init(console_recorder_options));

    LoggerPtr logger;
    Unwrap(logger, Logger::Init(options, std::move(cont)));
    ASSERT_TRUE(logger);
}

TEST(SclTest, ConsoleRecorderInitialized) {
    ConsoleRecorder::Options options{};
    const auto recorder = ConsoleRecorder::Init(options);
    ASSERT_TRUE(recorder);
}

TEST(SclTest, FileRecorderPathNotExistsError) {
    using Error = FileRecorder::InitError;
    FileRecorder::Options options{};
    // pass the options with the empty path
    const auto result = FileRecorder::Init(options);
    EXPECT_ERROR(result, Error::PathNotExists);
}

TEST(SclTest, FileRecorderEmptyTemplateFilenameError) {
    using Error = FileRecorder::InitError;
    FileRecorder::Options options{};
    options.log_directory = fs::current_path();

    // pass the options with the empty file name template
    const auto result = FileRecorder::Init(options);
    EXPECT_ERROR(result, Error::IncorrectFileNameTemplate);
}

TEST(SclTest, FileRecorderUnknownSpecifierError) {
    using Error = FileRecorder::InitError;
    FileRecorder::Options options{};
    options.log_directory = fs::current_path();
    // set the file name with unknown 'u' specifier
    options.file_name_template = "file.%u.txt";

    // pass the options with the empty file name template
    const auto result = FileRecorder::Init(options);
    EXPECT_ERROR(result, Error::IncorrectFileNameTemplate);
}

TEST(SclTest, FileRecorderDuplicateSpecifiersError) {
    using Error = FileRecorder::InitError;
    FileRecorder::Options options{};
    options.log_directory = fs::current_path();
    // set the file name with unknown 'u' specifier
    options.file_name_template = "file.%n.%n.txt";

    // pass the options with the empty file name template
    const auto result = FileRecorder::Init(options);
    EXPECT_ERROR(result, Error::IncorrectFileNameTemplate);
}
