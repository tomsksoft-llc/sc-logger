#include <gtest/gtest.h>
#include <cis1_core_logger/core_logger.h>
#include <cis1_core_logger/core_record.h>
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
using namespace cis1::core_logger;

TEST(SclTest, LoggerIncorrectLogLevelError) {
    using Error = CoreLogger::InitError;

    CoreLogger::Options options{static_cast<Level>(50)};
    RecordersCont<CoreRecord> cont;

    auto result = CoreLogger::Init(options, std::move(cont));
    EXPECT_ERROR(result, Error::IncorrectLogLevel);
}

TEST(SclTest, LoggerNoRecordersError) {
    using Error = CoreLogger::InitError;

    CoreLogger::Options options{};
    RecordersCont<CoreRecord> cont;

    auto result = CoreLogger::Init(options, std::move(cont));
    EXPECT_ERROR(result, Error::NoRecorders);
}

TEST(SclTest, LoggerUnallocatedRecorderError) {
    using Error = CoreLogger::InitError;

    CoreLogger::Options options{};
    RecordersCont<CoreRecord> cont;
    // push an unallocated recorder to the cont
    cont.push_back(std::unique_ptr<IRecorder<CoreRecord>>());

    auto result = CoreLogger::Init(options, std::move(cont));
    EXPECT_ERROR(result, Error::UnallocatedRecorder);
}

TEST(SclTest, LoggerInitialized) {
    using Error = CoreLogger::InitError;

    CoreLogger::Options options{Level::Debug};
    ConsoleRecorder<CoreRecord>::Options console_recorder_options{};

    RecordersCont<CoreRecord> cont;
    // push an unallocated recorder to the cont
    cont.push_back(ConsoleRecorder<CoreRecord>::Init(console_recorder_options));

    LoggerPtr logger;
    Unwrap(logger, CoreLogger::Init(options, std::move(cont)));
    ASSERT_TRUE(logger);
}

TEST(SclTest, ConsoleRecorderInitialized) {
    ConsoleRecorder<CoreRecord>::Options options{};
    const auto recorder = ConsoleRecorder<CoreRecord>::Init(options);
    ASSERT_TRUE(recorder);
}

TEST(SclTest, FileRecorderPathNotExistsError) {
    using Error = FileRecorder<CoreRecord>::InitError;
    FileRecorder<CoreRecord>::Options options{};
    // pass the options with the empty path
    const auto result = FileRecorder<CoreRecord>::Init(options);
    EXPECT_ERROR(result, Error::PathNotExists);
}

TEST(SclTest, FileRecorderEmptyTemplateFilenameError) {
    using Error = FileRecorder<CoreRecord>::InitError;
    FileRecorder<CoreRecord>::Options options{};
    options.log_directory = fs::current_path();

    // pass the options with the empty file name template
    const auto result = FileRecorder<CoreRecord>::Init(options);
    EXPECT_ERROR(result, Error::IncorrectFileNameTemplate);
}

TEST(SclTest, FileRecorderUnknownSpecifierError) {
    using Error = FileRecorder<CoreRecord>::InitError;
    FileRecorder<CoreRecord>::Options options{};
    options.log_directory = fs::current_path();
    // set the file name with unknown 'u' specifier
    options.file_name_template = "file.%u.txt";

    // pass the options with the empty file name template
    const auto result = FileRecorder<CoreRecord>::Init(options);
    EXPECT_ERROR(result, Error::IncorrectFileNameTemplate);
}

TEST(SclTest, FileRecorderDuplicateSpecifiersError) {
    using Error = FileRecorder<CoreRecord>::InitError;
    FileRecorder<CoreRecord>::Options options{};
    options.log_directory = fs::current_path();
    // set the file name with unknown 'u' specifier
    options.file_name_template = "file.%n.%n.txt";

    // pass the options with the empty file name template
    const auto result = FileRecorder<CoreRecord>::Init(options);
    EXPECT_ERROR(result, Error::IncorrectFileNameTemplate);
}
