//
// Configuration Module Unit Tests
//

#include <yass/config/config.hpp>
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

using namespace yass::config;
using yass::IoError;
using yass::ChessException;
namespace fs = std::filesystem;

class ConfigTest : public ::testing::Test {
protected:
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  fs::path fixtures_dir;

  void SetUp() override {
    // Get the path to the fixtures directory
    fixtures_dir = fs::current_path() / ".." / ".." / "tests" / "fixtures";

    // If not found, try alternate path (for different build configurations)
    if (!fs::exists(fixtures_dir)) {
      fixtures_dir = fs::current_path() / "tests" / "fixtures";
    }

    // Verify fixtures directory exists
    ASSERT_TRUE(fs::exists(fixtures_dir)) << "Fixtures directory not found at: " << fixtures_dir;
  }

  void TearDown() override {
    // Reset global state after each test
    yass_conf = site_config{};
    site_tags.clear();
    global_table_tags.clear();
    excluded_files.clear();
    site_directory.clear();
  }
};

// ============================================================================
// Valid Configuration Tests
// ============================================================================

TEST_F(ConfigTest, LoadValidConfiguration) {
  ASSERT_NO_THROW(load_site_config(fixtures_dir.string()));

  // Verify configuration fields
  EXPECT_EQ(yass_conf.server_port, 8080);
  EXPECT_EQ(yass_conf.server_address, "0.0.0.0");
  EXPECT_EQ(yass_conf.base_url, "https://example.com");
  EXPECT_EQ(yass_conf.site_name, "Test Site");
  EXPECT_EQ(yass_conf.site_language, "en");
  EXPECT_EQ(yass_conf.site_description, "A test site for unit testing");
  EXPECT_EQ(yass_conf.site_author, "Test Author");
  EXPECT_EQ(yass_conf.monitor_interval, 500);
  EXPECT_TRUE(yass_conf.auto_build);
  EXPECT_FALSE(yass_conf.verbose_output);
  EXPECT_EQ(yass_conf.default_template, "default.html");
  EXPECT_EQ(yass_conf.default_layout, "page.html");
}

TEST_F(ConfigTest, DirectoryPathsAreNormalized) {
  load_site_config(fixtures_dir.string());

  // All directory paths should be absolute
  EXPECT_TRUE(fs::path(yass_conf.layout_directory).is_absolute());
  EXPECT_TRUE(fs::path(yass_conf.output_directory).is_absolute());
  EXPECT_TRUE(fs::path(yass_conf.modules_directory).is_absolute());
  EXPECT_TRUE(fs::path(yass_conf.content_directory).is_absolute());

  // Paths should be normalized (no relative components)
  EXPECT_EQ(fs::path(yass_conf.layout_directory).filename().string(), "layouts");
  EXPECT_EQ(fs::path(yass_conf.output_directory).filename().string(), "_output");
}

TEST_F(ConfigTest, SimpleTagsAreStored) {
  load_site_config(fixtures_dir.string());

  // Check simple tags
  EXPECT_EQ(site_tags["CustomTag1"], "value1");
  EXPECT_EQ(site_tags["CustomTag2"], "value2");
  EXPECT_EQ(site_tags["BooleanTag"], "true");
  EXPECT_EQ(site_tags["NumericTag"], "42");

  // Special tags (BaseURL, Name, Language) should be in both config and tags
  EXPECT_EQ(site_tags["BaseURL"], "https://example.com");
  EXPECT_EQ(site_tags["Name"], "Test Site");
  EXPECT_EQ(site_tags["Language"], "en");
}

TEST_F(ConfigTest, TableTagsAreStored) {
  load_site_config(fixtures_dir.string());

  // Check table tags
  ASSERT_TRUE(global_table_tags.contains("Authors"));
  auto& authors = global_table_tags["Authors"];
  ASSERT_EQ(authors.size(), 3);
  EXPECT_EQ(authors[0], "Alice");
  EXPECT_EQ(authors[1], "Bob");
  EXPECT_EQ(authors[2], "Charlie");

  ASSERT_TRUE(global_table_tags.contains("Categories"));
  auto& categories = global_table_tags["Categories"];
  ASSERT_EQ(categories.size(), 3);
  EXPECT_EQ(categories[0], "Programming");
  EXPECT_EQ(categories[1], "Chess");
  EXPECT_EQ(categories[2], "Testing");
}

TEST_F(ConfigTest, ExcludedFilesArePopulated) {
  load_site_config(fixtures_dir.string());

  // Check excluded files
  // NOLINTNEXTLINE(modernize-use-ranges)
  EXPECT_TRUE(std::find(excluded_files.begin(), excluded_files.end(), ".") != excluded_files.end());
  // NOLINTNEXTLINE(modernize-use-ranges)
  EXPECT_TRUE(std::find(excluded_files.begin(), excluded_files.end(), "..") != excluded_files.end());
  // NOLINTNEXTLINE(modernize-use-ranges)
  EXPECT_TRUE(std::find(excluded_files.begin(), excluded_files.end(), "site.cfg") != excluded_files.end());
  // NOLINTNEXTLINE(modernize-use-ranges)
  EXPECT_TRUE(std::find(excluded_files.begin(), excluded_files.end(), "layouts") != excluded_files.end());
  // NOLINTNEXTLINE(modernize-use-ranges)
  EXPECT_TRUE(std::find(excluded_files.begin(), excluded_files.end(), "_output") != excluded_files.end());
  // NOLINTNEXTLINE(modernize-use-ranges)
  EXPECT_TRUE(std::find(excluded_files.begin(), excluded_files.end(), "modules") != excluded_files.end());
}

TEST_F(ConfigTest, SiteDirectoryIsSet) {
  load_site_config(fixtures_dir.string());

  EXPECT_FALSE(site_directory.empty());
  EXPECT_TRUE(fs::path(site_directory).is_absolute());
}

TEST_F(ConfigTest, MinimalConfigurationWithDefaults) {
  // Create a temporary directory with minimal config
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_minimal";
  fs::create_directories(temp_dir);

  // Copy minimal config
  fs::copy_file(
    fixtures_dir / "minimal_site.cfg",
    temp_dir / "site.cfg",
    fs::copy_options::overwrite_existing
  );

  ASSERT_NO_THROW(load_site_config(temp_dir.string()));

  // Check that defaults are applied
  EXPECT_EQ(yass_conf.layout_directory, (temp_dir / "layouts").string());
  EXPECT_EQ(yass_conf.output_directory, (temp_dir / "_output").string());
  EXPECT_EQ(yass_conf.server_port, 8000);  // Default
  EXPECT_EQ(yass_conf.server_address, "localhost");  // Default
  EXPECT_EQ(yass_conf.site_name, "Minimal Site");

  // Cleanup
  fs::remove_all(temp_dir);
}

TEST_F(ConfigTest, CommentsAndEmptyLinesAreIgnored) {
  load_site_config(fixtures_dir.string());

  // If we got here without throwing, comments were successfully ignored
  SUCCEED();
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(ConfigTest, MissingConfigFileThrowsIoError) {
  fs::path nonexistent = fs::temp_directory_path() / "nonexistent_yass_dir";

  EXPECT_THROW({
    try {
      load_site_config(nonexistent.string());
    } catch (const IoError& e) {
      EXPECT_NE(std::string(e.what()).find("Cannot open"), std::string::npos);
      throw;
    }
  }, IoError);
}

TEST_F(ConfigTest, InvalidLineThrowsConfigurationError) {
  // Create temp directory with invalid config
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_invalid";
  fs::create_directories(temp_dir);

  // Copy invalid config
  fs::copy_file(
    fixtures_dir / "invalid_site.cfg",
    temp_dir / "site.cfg",
    fs::copy_options::overwrite_existing
  );

  EXPECT_THROW({
    try {
      load_site_config(temp_dir.string());
    } catch (const ConfigurationError& e) {
      std::string msg(e.what());
      EXPECT_NE(msg.find("Missing '='"), std::string::npos);
      throw;
    }
  }, ConfigurationError);

  // Cleanup
  fs::remove_all(temp_dir);
}

TEST_F(ConfigTest, OutOfRangePortThrowsConfigurationError) {
  // Create temp directory
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_port";
  fs::create_directories(temp_dir);

  // Create config with invalid port
  std::ofstream config(temp_dir / "site.cfg");
  config << "ServerPort = 99999\n";  // Out of range
  config.close();

  EXPECT_THROW({
    try {
      load_site_config(temp_dir.string());
    } catch (const ConfigurationError& e) {
      std::string msg(e.what());
      EXPECT_NE(msg.find("ServerPort"), std::string::npos);
      EXPECT_NE(msg.find("65535"), std::string::npos);
      throw;
    }
  }, ConfigurationError);

  // Cleanup
  fs::remove_all(temp_dir);
}

TEST_F(ConfigTest, InvalidBooleanThrowsConfigurationError) {
  // Create temp directory
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_bool";
  fs::create_directories(temp_dir);

  // Create config with invalid boolean
  std::ofstream config(temp_dir / "site.cfg");
  config << "AutoBuild = maybe\n";  // Invalid boolean
  config.close();

  EXPECT_THROW({
    try {
      load_site_config(temp_dir.string());
    } catch (const ConfigurationError& e) {
      std::string msg(e.what());
      EXPECT_NE(msg.find("Invalid boolean"), std::string::npos);
      throw;
    }
  }, ConfigurationError);

  // Cleanup
  fs::remove_all(temp_dir);
}

TEST_F(ConfigTest, BooleanParsingIsCaseInsensitive) {
  // Create temp directory
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_bool_case";
  fs::create_directories(temp_dir);

  // Test various boolean cases
  std::ofstream config(temp_dir / "site.cfg");
  config << "AutoBuild = TRUE\n";
  config << "VerboseOutput = False\n";
  config.close();

  ASSERT_NO_THROW(load_site_config(temp_dir.string()));
  EXPECT_TRUE(yass_conf.auto_build);
  EXPECT_FALSE(yass_conf.verbose_output);

  // Cleanup
  fs::remove_all(temp_dir);
}

TEST_F(ConfigTest, WhitespaceTrimmingWorks) {
  // Create temp directory
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_whitespace";
  fs::create_directories(temp_dir);

  // Create config with lots of whitespace
  std::ofstream config(temp_dir / "site.cfg");
  config << "  Name  =  Whitespace Test  \n";
  config << "\tServerPort\t=\t9000\t\n";
  config.close();

  ASSERT_NO_THROW(load_site_config(temp_dir.string()));
  EXPECT_EQ(yass_conf.site_name, "Whitespace Test");
  EXPECT_EQ(yass_conf.server_port, 9000);

  // Cleanup
  fs::remove_all(temp_dir);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_F(ConfigTest, MultipleLoadsResetState) {
  load_site_config(fixtures_dir.string());
  EXPECT_EQ(yass_conf.site_name, "Test Site");

  // Create minimal config
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_reload";
  fs::create_directories(temp_dir);
  fs::copy_file(
    fixtures_dir / "minimal_site.cfg",
    temp_dir / "site.cfg",
    fs::copy_options::overwrite_existing
  );

  load_site_config(temp_dir.string());
  EXPECT_EQ(yass_conf.site_name, "Minimal Site");

  // Old tags should be cleared
  EXPECT_FALSE(site_tags.contains("CustomTag1"));

  // Cleanup
  fs::remove_all(temp_dir);
}

TEST_F(ConfigTest, EmptyTableTagIsInitialized) {
  // Create config with empty table tag
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_empty_table";
  fs::create_directories(temp_dir);

  std::ofstream config(temp_dir / "site.cfg");
  config << "EmptyList = []\n";
  config.close();

  ASSERT_NO_THROW(load_site_config(temp_dir.string()));

  ASSERT_TRUE(global_table_tags.contains("EmptyList"));
  EXPECT_TRUE(global_table_tags["EmptyList"].empty());

  // Cleanup
  fs::remove_all(temp_dir);
}

// ============================================================================
// Security Tests - Path Traversal Protection
// ============================================================================

TEST_F(ConfigTest, AbsolutePathsAreRejected) {
  // Create temp directory with config containing absolute path
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_absolute_path";
  fs::create_directories(temp_dir);

  std::ofstream config(temp_dir / "site.cfg");
  config << "LayoutDirectory = /etc/passwd\n";
  config.close();

  EXPECT_THROW({
    try {
      load_site_config(temp_dir.string());
    } catch (const ConfigurationError& e) {
      std::string msg(e.what());
      EXPECT_NE(msg.find("Absolute paths not allowed"), std::string::npos);
      throw;
    }
  }, ConfigurationError);

  // Cleanup
  fs::remove_all(temp_dir);
}

TEST_F(ConfigTest, PathTraversalIsDetected) {
  // Create temp directory with config containing path traversal
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_traversal";
  fs::create_directories(temp_dir);

  std::ofstream config(temp_dir / "site.cfg");
  config << "LayoutDirectory = ../../../etc\n";
  config.close();

  EXPECT_THROW({
    try {
      load_site_config(temp_dir.string());
    } catch (const ConfigurationError& e) {
      std::string msg(e.what());
      EXPECT_NE(msg.find("Path traversal detected"), std::string::npos);
      throw;
    }
  }, ConfigurationError);

  // Cleanup
  fs::remove_all(temp_dir);
}

TEST_F(ConfigTest, ValidRelativePathsAreAccepted) {
  // Create temp directory with valid relative paths
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_valid_relative";
  fs::create_directories(temp_dir);

  std::ofstream config(temp_dir / "site.cfg");
  config << "LayoutDirectory = my_layouts\n";
  config << "OutputDirectory = output/html\n";
  config.close();

  ASSERT_NO_THROW(load_site_config(temp_dir.string()));

  // Verify paths are normalized to absolute
  EXPECT_TRUE(fs::path(yass_conf.layout_directory).is_absolute());
  EXPECT_TRUE(fs::path(yass_conf.output_directory).is_absolute());

  // Verify paths contain the expected directory names
  EXPECT_EQ(fs::path(yass_conf.layout_directory).filename().string(), "my_layouts");
  EXPECT_EQ(fs::path(yass_conf.output_directory).filename().string(), "html");

  // Cleanup
  fs::remove_all(temp_dir);
}

// ============================================================================
// Edge Case Tests - Table Tags
// ============================================================================

TEST_F(ConfigTest, TableTagContextReset) {
  // Create config with table tag followed by non-table key
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_table_context";
  fs::create_directories(temp_dir);

  std::ofstream config(temp_dir / "site.cfg");
  config << "Authors = []\n";
  config << "Authors = Alice\n";
  config << "Authors = Bob\n";
  config << "Name = MySite\n";  // This should reset table tag context
  config << "Authors = Charlie\n";  // This should NOT be added to Authors table
  config.close();

  ASSERT_NO_THROW(load_site_config(temp_dir.string()));

  // Authors table should have only Alice and Bob
  ASSERT_TRUE(global_table_tags.contains("Authors"));
  auto& authors = global_table_tags["Authors"];
  EXPECT_EQ(authors.size(), 2);
  EXPECT_EQ(authors[0], "Alice");
  EXPECT_EQ(authors[1], "Bob");

  // Charlie should NOT be in Authors
  // NOLINTNEXTLINE(modernize-use-ranges)
  EXPECT_TRUE(std::find(authors.begin(), authors.end(), "Charlie") == authors.end());

  // Name should be set correctly
  EXPECT_EQ(yass_conf.site_name, "MySite");

  // Cleanup
  fs::remove_all(temp_dir);
}

TEST_F(ConfigTest, InterleavedTableTags) {
  // Create config with multiple table tags interleaved with simple tags
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_interleaved";
  fs::create_directories(temp_dir);

  std::ofstream config(temp_dir / "site.cfg");
  config << "Name = TestSite\n";
  config << "Authors = []\n";
  config << "Authors = Alice\n";
  config << "Description = A test site\n";
  config << "Categories = []\n";
  config << "Categories = Tech\n";
  config << "Categories = Science\n";
  config << "ServerPort = 8080\n";
  config.close();

  ASSERT_NO_THROW(load_site_config(temp_dir.string()));

  // Verify Authors table
  ASSERT_TRUE(global_table_tags.contains("Authors"));
  auto& authors = global_table_tags["Authors"];
  EXPECT_EQ(authors.size(), 1);
  EXPECT_EQ(authors[0], "Alice");

  // Verify Categories table
  ASSERT_TRUE(global_table_tags.contains("Categories"));
  auto& categories = global_table_tags["Categories"];
  EXPECT_EQ(categories.size(), 2);
  EXPECT_EQ(categories[0], "Tech");
  EXPECT_EQ(categories[1], "Science");

  // Verify simple tags
  EXPECT_EQ(yass_conf.site_name, "TestSite");
  EXPECT_EQ(yass_conf.site_description, "A test site");
  EXPECT_EQ(yass_conf.server_port, 8080);

  // Cleanup
  fs::remove_all(temp_dir);
}

// ============================================================================
// Missing Test Coverage - Issue #1: Empty Key Validation
// ============================================================================

TEST_F(ConfigTest, EmptyKeyThrowsConfigurationError) {
  // Create config with empty key (= Value with nothing before equals)
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_empty_key";
  fs::create_directories(temp_dir);

  std::ofstream config(temp_dir / "site.cfg");
  config << "Name = ValidSite\n";
  config << "= InvalidValue\n";  // Empty key
  config.close();

  EXPECT_THROW({
    try {
      load_site_config(temp_dir.string());
    } catch (const ConfigurationError& e) {
      std::string msg(e.what());
      EXPECT_NE(msg.find("Empty key"), std::string::npos);
      throw;
    }
  }, ConfigurationError);

  // Cleanup
  fs::remove_all(temp_dir);
}

// ============================================================================
// Missing Test Coverage - Issue #2: Line Number Reporting in Error Messages
// ============================================================================

TEST_F(ConfigTest, ErrorMessagesIncludeLineNumbers) {
  // Test that error messages include line numbers for various errors

  // Test 1: Missing equals sign on line 3
  {
    fs::path temp_dir = fs::temp_directory_path() / "yass_test_line_num_1";
    fs::create_directories(temp_dir);

    std::ofstream config(temp_dir / "site.cfg");
    config << "Name = TestSite\n";      // Line 1
    config << "Description = Test\n";   // Line 2
    config << "InvalidLine\n";          // Line 3 - error here
    config.close();

    EXPECT_THROW({
      try {
        load_site_config(temp_dir.string());
      } catch (const ConfigurationError& e) {
        std::string msg(e.what());
        EXPECT_NE(msg.find("Line 3"), std::string::npos);
        EXPECT_NE(msg.find("Missing '='"), std::string::npos);
        throw;
      }
    }, ConfigurationError);

    fs::remove_all(temp_dir);
  }

  // Test 2: Out of range port on line 2
  {
    fs::path temp_dir = fs::temp_directory_path() / "yass_test_line_num_2";
    fs::create_directories(temp_dir);

    std::ofstream config(temp_dir / "site.cfg");
    config << "Name = TestSite\n";      // Line 1
    config << "ServerPort = 99999\n";   // Line 2 - error here
    config.close();

    EXPECT_THROW({
      try {
        load_site_config(temp_dir.string());
      } catch (const ConfigurationError& e) {
        std::string msg(e.what());
        EXPECT_NE(msg.find("Line 2"), std::string::npos);
        EXPECT_NE(msg.find("ServerPort"), std::string::npos);
        throw;
      }
    }, ConfigurationError);

    fs::remove_all(temp_dir);
  }

  // Test 3: Empty key on line 4
  {
    fs::path temp_dir = fs::temp_directory_path() / "yass_test_line_num_3";
    fs::create_directories(temp_dir);

    std::ofstream config(temp_dir / "site.cfg");
    config << "Name = TestSite\n";        // Line 1
    config << "# Comment line\n";         // Line 2
    config << "Description = Valid\n";    // Line 3
    config << "= EmptyKey\n";             // Line 4 - error here
    config.close();

    EXPECT_THROW({
      try {
        load_site_config(temp_dir.string());
      } catch (const ConfigurationError& e) {
        std::string msg(e.what());
        EXPECT_NE(msg.find("Line 4"), std::string::npos);
        EXPECT_NE(msg.find("Empty key"), std::string::npos);
        throw;
      }
    }, ConfigurationError);

    fs::remove_all(temp_dir);
  }
}

// ============================================================================
// Missing Test Coverage - Issue #3: MonitorInterval Validation
// ============================================================================

TEST_F(ConfigTest, InvalidMonitorIntervalThrowsConfigurationError) {
  // Test 1: Non-numeric MonitorInterval
  {
    fs::path temp_dir = fs::temp_directory_path() / "yass_test_monitor_nonnumeric";
    fs::create_directories(temp_dir);

    std::ofstream config(temp_dir / "site.cfg");
    config << "MonitorInterval = invalid\n";
    config.close();

    EXPECT_THROW({
      try {
        load_site_config(temp_dir.string());
      } catch (const ConfigurationError& e) {
        std::string msg(e.what());
        EXPECT_NE(msg.find("MonitorInterval"), std::string::npos);
        throw;
      }
    }, ConfigurationError);

    fs::remove_all(temp_dir);
  }

  // Test 2: Negative MonitorInterval
  {
    fs::path temp_dir = fs::temp_directory_path() / "yass_test_monitor_negative";
    fs::create_directories(temp_dir);

    std::ofstream config(temp_dir / "site.cfg");
    config << "MonitorInterval = -1\n";
    config.close();

    EXPECT_THROW({
      try {
        load_site_config(temp_dir.string());
      } catch (const ConfigurationError& e) {
        std::string msg(e.what());
        EXPECT_NE(msg.find("MonitorInterval"), std::string::npos);
        EXPECT_NE(msg.find("between 0 and"), std::string::npos);
        throw;
      }
    }, ConfigurationError);

    fs::remove_all(temp_dir);
  }

  // Test 3: MonitorInterval exceeding UINT32_MAX
  {
    fs::path temp_dir = fs::temp_directory_path() / "yass_test_monitor_overflow";
    fs::create_directories(temp_dir);

    std::ofstream config(temp_dir / "site.cfg");
    config << "MonitorInterval = 4294967296\n";  // UINT32_MAX + 1
    config.close();

    EXPECT_THROW({
      try {
        load_site_config(temp_dir.string());
      } catch (const ConfigurationError& e) {
        std::string msg(e.what());
        EXPECT_NE(msg.find("MonitorInterval"), std::string::npos);
        EXPECT_NE(msg.find("4294967295"), std::string::npos);  // UINT32_MAX
        throw;
      }
    }, ConfigurationError);

    fs::remove_all(temp_dir);
  }
}

TEST_F(ConfigTest, ValidMonitorIntervalAccepted) {
  // Test valid MonitorInterval values
  fs::path temp_dir = fs::temp_directory_path() / "yass_test_monitor_valid";
  fs::create_directories(temp_dir);

  std::ofstream config(temp_dir / "site.cfg");
  config << "MonitorInterval = 300\n";
  config.close();

  ASSERT_NO_THROW(load_site_config(temp_dir.string()));
  EXPECT_EQ(yass_conf.monitor_interval, 300);

  // Cleanup
  fs::remove_all(temp_dir);
}

// ============================================================================
// Missing Test Coverage - Issue #4: Exception Propagation to Caller
// ============================================================================

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_F(ConfigTest, ExceptionsPropagateToCallerCorrectly) {
  // Test 1: ConfigurationError propagates through wrapper for invalid file
  {
    fs::path temp_dir = fs::temp_directory_path() / "yass_test_propagation_1";
    fs::create_directories(temp_dir);

    std::ofstream config(temp_dir / "site.cfg");
    config << "InvalidLine\n";
    config.close();

    // Wrapper that catches and rethrows
    EXPECT_THROW({
      try {
        load_site_config(temp_dir.string());
      } catch (const ConfigurationError&) {
        throw;  // Exception properly propagated
      }
    }, ConfigurationError);

    fs::remove_all(temp_dir);
  }

  // Test 2: IoError propagates through wrapper for missing file
  {
    fs::path temp_dir = fs::temp_directory_path() / "yass_test_propagation_2";
    fs::create_directories(temp_dir);
    // Don't create site.cfg - file is missing

    // Wrapper that catches and rethrows
    EXPECT_THROW({
      try {
        load_site_config(temp_dir.string());
      } catch (const IoError&) {
        throw;  // Exception properly propagated
      }
    }, IoError);

    fs::remove_all(temp_dir);
  }

  // Test 3: No exception when config is valid
  {
    fs::path temp_dir = fs::temp_directory_path() / "yass_test_propagation_3";
    fs::create_directories(temp_dir);

    std::ofstream config(temp_dir / "site.cfg");
    config << "Name = ValidSite\n";
    config.close();

    // Should not throw
    EXPECT_NO_THROW({
      load_site_config(temp_dir.string());
    });

    fs::remove_all(temp_dir);
  }
}
