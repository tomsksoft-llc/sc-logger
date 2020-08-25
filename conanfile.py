from conans import ConanFile, CMake, tools

class ScLogger(ConanFile):
    name = "sc_logger"
    version = "1.0.4"
    description = "Self check logger"
    author = "BoykoSO <bso@tomsksoft.com>"
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_testing": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_testing": False,
    }
    _source_subfolder = "source_subfolder"

    def requirements(self):
        if self.options.build_testing:
            self.requires("gtest/1.8.1@bincrafters/stable")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_TESTING"] = self.options.build_testing
        cmake.configure(source_folder = self._source_subfolder)
        return cmake

    def source(self):
        git = tools.Git(folder=self._source_subfolder)
        git.clone("https://github.com/tomsksoft-llc/sc-logger")
        git.checkout("v%s" % self.version);

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package2(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("libsc_logger.a", dst="lib")
        self.copy("libsc_logger.lib", dst="lib")
        self.copy("libsc_logger.so", dst="lib")
        self.copy("libsc_logger.dll", dst="lib")

    def package_info(self):
        self.cpp_info.libs = ["sc_logger"]

    def imports(self):
        self.copy("FindFilesystem.cmake", dst="cmake/modules", src="cmake/modules")
