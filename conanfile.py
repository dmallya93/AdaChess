from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain


class AdaChessConan(ConanFile):
    name = "adachess"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("gtest/1.14.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.1")

    def configure(self):
        # Configure options for dependencies
        self.options["gtest"].shared = False

        # Optimize for safety-critical code
        if self.settings.build_type == "Debug":
            self.options["gtest"].build_gmock = True

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()
