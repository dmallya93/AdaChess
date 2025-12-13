//
// Configuration Module for YASS (Yet Another Static Site generator)
//
// This module handles loading and managing site configuration from site.cfg files
//
// DESIGN DECISION #1: Configuration Storage and Parsing Strategy
// Decision: Custom C++ containers (std::unordered_map) with manual INI parsing
// Rationale: Zero dependencies, type-safe separation, direct map access performance
// See DESIGN_DECISIONS.md for full details
//
// DESIGN DECISION #2: String Handling and Path Management
// Decision: std::string + std::filesystem with std::string_view for parameters
// Rationale: Standard library only, cross-platform, canonical() for security
// See DESIGN_DECISIONS.md for full details
//

#pragma once

#include <yass/error.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace yass::config {

/// Configuration structure for site settings
struct site_config {
  // Directory paths
  std::string layout_directory{"layouts"};
  std::string output_directory{"_output"};
  std::string modules_directory{"modules"};
  std::string content_directory{"content"};

  // Server settings
  std::uint16_t server_port{8000};
  std::string server_address{"localhost"};

  // Site metadata
  std::string base_url;
  std::string site_name;
  std::string site_language{"en"};
  std::string site_description;
  std::string site_author;

  // Monitoring and build settings
  std::uint32_t monitor_interval{1000};  // milliseconds
  bool auto_build{true};
  bool verbose_output{false};

  // Template settings
  std::string default_template{"default.html"};
  std::string default_layout{"page.html"};
};

/// Simple tags container (string key-value pairs)
using simple_tags_map = std::unordered_map<std::string, std::string>;

/// Table tags container (string key to vector of strings)
using table_tags_map = std::unordered_map<std::string, std::vector<std::string>>;

/// Excluded files list
using excluded_files_list = std::vector<std::string>;

// Global configuration objects
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp)
extern site_config yass_conf;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern simple_tags_map site_tags;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern table_tags_map global_table_tags;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern excluded_files_list excluded_files;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern std::string site_directory;

/// Load site configuration from a directory containing site.cfg
///
/// Reads and parses site.cfg file in the specified directory
/// Populates global configuration objects (yass_conf, site_tags, global_table_tags)
/// Normalizes paths relative to the site directory
/// Automatically excludes special directories and files
///
/// @param directory_name Path to directory containing site.cfg
/// @throws IoError if site.cfg cannot be read
/// @throws ConfigurationError if site.cfg has invalid format or values
void load_site_config(const std::string& directory_name);

/// Configuration-specific exception
class ConfigurationError : public ChessException {
public:
  explicit ConfigurationError(const std::string& message)
    : ChessException(message) {}
};

}  // namespace yass::config
