from conans.model.conan_file import ConanFile
from conans import CMake


class Cis1CoreNativeTests(ConanFile):
    name = "sc_logger"
    version = "0.0.1"
    description = "Self check logger"
    author = "BoykoSO <bso@tomsksoft.com>"
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"
    requires = "gtest/1.8.1@bincrafters/stable"

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.copy("*.h", dst="include/sc_logger", src="include")
        self.copy("libsc_logger.a", dst="lib", src="lib")
        self.copy("libsc_logger.lib", dst="lib", src="lib")

    def package_info(self):
        self.cpp_info.libs = ["sc_logger"]

    def imports(self):
        self.copy("FindFilesystem.cmake", dst="cmake/modules", src="cmake/modules")

