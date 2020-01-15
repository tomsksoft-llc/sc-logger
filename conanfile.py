from conans.model.conan_file import ConanFile
from conans import CMake


class ScLogger(ConanFile):
    name = "sc_logger"
    version = "1.0.1"
    description = "Self check logger"
    author = "BoykoSO <bso@tomsksoft.com>"
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"
    exports = []
    exports_sources = [
        "CMakeLists.txt",
        "include/*",
        "src/*",
        "Doxyfile.in",
        "version.txt",
        "cmake/*",
        "test_package/*",
    ]
    requires = "gtest/1.8.1@bincrafters/stable"

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("libsc_logger.a", dst="lib")
        self.copy("libsc_logger.lib", dst="lib")

    def package_info(self):
        self.cpp_info.libs = ["sc_logger"]

    def imports(self):
        self.copy("FindFilesystem.cmake", dst="cmake/modules", src="cmake/modules")
