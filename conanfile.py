from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain


class AdaChessConan(ConanFile):
    name = "adachess"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        # Core testing framework (required)
        self.requires("gtest/1.14.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.1")

    def configure(self):
        # Optimize for safety-critical code
        # Static linking for better analysis and reproducibility
        self.options["gtest"].shared = False

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        # Enable clang-tidy by default
        tc.variables["ENABLE_CLANG_TIDY"] = "ON"
        tc.generate()
