#include <gtest/gtest.h>
#include <scl/logger.h>

namespace fs = std::filesystem;

TEST(SclTest, Initialization) {
    scl::Options options;
    options.log_path = "/home/bso/tmp/";
    options.file_name_template = "file.txt";
    options.module_name = "test_module";
    options.parent_pid = 1;
    options.session_id = 123;
    options.level = scl::Level::Debug;
    options.use_console_output = true;
    options.align_info = scl::AlignInfo(10, 10);
    const scl::InitResult result = scl::Logger::Init(options);

    LOG_D("foo %s", "bar");    
}

