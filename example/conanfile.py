from conans.model.conan_file import ConanFile
from conans import CMake


class Cis1CoreNativeTests(ConanFile):
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def imports(self):
        self.copy("libsc_logger.a", dst="lib", src="lib")
        self.copy("libsc_logger.lib", dst="lib", src="lib")
        self.copy("FindFilesystem.cmake", dst="cmake/modules", src="cmake/modules")

