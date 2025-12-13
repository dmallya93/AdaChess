//
// Configuration Module Implementation
//
// Implements Design Decision #1 (Configuration Storage) and #2 (Path Handling)
// See DESIGN_DECISIONS.md for rationale
//

#include <yass/config/config.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace yass::config {

// Global configuration objects
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,cert-err58-cpp)
site_config yass_conf;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
simple_tags_map site_tags;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
table_tags_map global_table_tags;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
excluded_files_list excluded_files;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::string site_directory;

namespace {

// Trim whitespace from both ends of a string
std::string trim(const std::string& str) {
  // NOLINTNEXTLINE(modernize-use-ranges)
  auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
    return std::isspace(ch);
  });
  // NOLINTNEXTLINE(modernize-use-ranges)
  auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
    return std::isspace(ch);
  }).base();

  return (start < end) ? std::string(start, end) : std::string();
}

// Convert string to lowercase
std::string to_lower(std::string str) {
  // NOLINTNEXTLINE(modernize-use-ranges)
  std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return str;
}

// Parse boolean value (case-insensitive "true" or "false")
bool parse_bool(const std::string& value) {
  std::string lower = to_lower(trim(value));
  if (lower == "true") {
    return true;
  }
  if (lower == "false") {
    return false;
  }
  throw ConfigurationError("Invalid boolean value: '" + value + "' (expected 'true' or 'false')");
}

// Parse unsigned 16-bit integer with range validation
std::uint16_t parse_uint16(const std::string& value, const std::string& key) {
  try {
    int num = std::stoi(trim(value));
    if (num < 1 || num > 65535) {
      throw ConfigurationError(key + " must be between 1 and 65535, got: " + value);
    }
    return static_cast<std::uint16_t>(num);
  } catch (const std::invalid_argument&) {
    throw ConfigurationError("Invalid number for " + key + ": " + value);
  } catch (const std::out_of_range&) {
    throw ConfigurationError(key + " value out of range: " + value);
  }
}

// Parse unsigned 32-bit integer
std::uint32_t parse_uint32(const std::string& value, const std::string& key) {
  try {
    long long num = std::stoll(trim(value));
    // NOLINTNEXTLINE(modernize-use-integer-sign-comparison)
    if (num < 0 || num > UINT32_MAX) {
      throw ConfigurationError(
        key + " must be between 0 and " + std::to_string(UINT32_MAX) + ", got: " + value
      );
    }
    return static_cast<std::uint32_t>(num);
  } catch (const std::invalid_argument&) {
    throw ConfigurationError("Invalid number for " + key + ": " + value);
  } catch (const std::out_of_range&) {
    throw ConfigurationError(key + " value out of range: " + value);
  }
}

// Normalize path (convert relative to absolute)
std::string normalize_path(const std::string& path, const std::string& base_dir) {
  namespace fs = std::filesystem;
  fs::path p(path);

  if (p.is_absolute()) {
    throw ConfigurationError("Absolute paths not allowed in configuration: " + path);
  }

  // Relative path - make absolute relative to base_dir
  fs::path base = fs::canonical(base_dir);
  fs::path absolute = (base / p).lexically_normal();

  // Use lexically_relative to verify path is within base directory
  // This properly handles path boundaries and prevents prefix bypass
  fs::path relative = absolute.lexically_relative(base);

  // Check if relative path escapes the base directory
  if (relative.empty() || (!relative.native().empty() && *relative.begin() == "..")) {
    throw ConfigurationError("Path traversal detected: " + path);
  }

  // Verify path doesn't have path separator issues
  auto base_str = base.string();
  auto result_str = absolute.string();

  if (result_str.size() < base_str.size() ||
      // NOLINTNEXTLINE(modernize-use-starts-ends-with)
      result_str.substr(0, base_str.size()) != base_str ||
      (result_str.size() > base_str.size() &&
       result_str[base_str.size()] != '/' &&
       result_str[base_str.size()] != '\\')) {
    throw ConfigurationError("Path traversal detected: " + path);
  }

  return result_str;
}

}  // anonymous namespace

void load_site_config(const std::string& directory_name) {
  namespace fs = std::filesystem;

  // Reset global state
  yass_conf = site_config{};
  site_tags.clear();
  global_table_tags.clear();
  excluded_files.clear();

  // Store and normalize site directory
  site_directory = fs::absolute(directory_name).string();

  // Open site.cfg file
  fs::path config_file = fs::path(site_directory) / "site.cfg";
  std::ifstream file(config_file);

  if (!file.is_open()) {
    throw IoError("Cannot open configuration file: " + config_file.string());
  }

  constexpr size_t max_line_length = 65536;  // 64KB limit
  std::string line;
  line.reserve(1024);
  int line_number = 0;
  std::string current_table_tag;

  while (std::getline(file, line)) {
    ++line_number;

    // Protect against DoS via unbounded line length
    if (line.size() > max_line_length) {
      throw ConfigurationError(
        "Line " + std::to_string(line_number) + ": Line too long (max " +
        std::to_string(max_line_length) + " bytes)"
      );
    }

    line = trim(line);

    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Find '=' separator
    size_t eq_pos = line.find('=');
    if (eq_pos == std::string::npos) {
      throw ConfigurationError(
        "Line " + std::to_string(line_number) + ": Missing '=' separator in: " + line
      );
    }

    std::string key = trim(line.substr(0, eq_pos));
    std::string value = trim(line.substr(eq_pos + 1));

    if (key.empty()) {
      throw ConfigurationError(
        "Line " + std::to_string(line_number) + ": Empty key before '='"
      );
    }

    // Check for table tag initialization
    if (value == "[]") {
      current_table_tag = key;
      global_table_tags[key] = std::vector<std::string>();
      continue;
    }

    // If we're in a table tag, add to its vector
    if (!current_table_tag.empty() && key == current_table_tag) {
      global_table_tags[current_table_tag].push_back(value);
      continue;
    }

    // Otherwise, it's either a configuration field or a simple tag
    current_table_tag.clear();  // Reset table tag context

    // Try to match known configuration fields
    if (key == "LayoutDirectory") {
      yass_conf.layout_directory = value;
    } else if (key == "OutputDirectory") {
      yass_conf.output_directory = value;
    } else if (key == "ModulesDirectory") {
      yass_conf.modules_directory = value;
    } else if (key == "ContentDirectory") {
      yass_conf.content_directory = value;
    } else if (key == "ServerPort") {
      try {
        yass_conf.server_port = parse_uint16(value, key);
      } catch (const ConfigurationError& e) {
        throw ConfigurationError(
          "Line " + std::to_string(line_number) + ": " + std::string(e.what())
        );
      }
    } else if (key == "ServerAddress") {
      yass_conf.server_address = value;
    } else if (key == "BaseURL") {
      yass_conf.base_url = value;
      site_tags[key] = value;  // Also store in simple tags
    } else if (key == "Name") {
      yass_conf.site_name = value;
      site_tags[key] = value;  // Also store in simple tags
    } else if (key == "Language") {
      yass_conf.site_language = value;
      site_tags[key] = value;  // Also store in simple tags
    } else if (key == "Description") {
      yass_conf.site_description = value;
    } else if (key == "Author") {
      yass_conf.site_author = value;
    } else if (key == "MonitorInterval") {
      try {
        yass_conf.monitor_interval = parse_uint32(value, key);
      } catch (const ConfigurationError& e) {
        throw ConfigurationError(
          "Line " + std::to_string(line_number) + ": " + std::string(e.what())
        );
      }
    } else if (key == "AutoBuild") {
      yass_conf.auto_build = parse_bool(value);
    } else if (key == "VerboseOutput") {
      yass_conf.verbose_output = parse_bool(value);
    } else if (key == "DefaultTemplate") {
      yass_conf.default_template = value;
    } else if (key == "DefaultLayout") {
      yass_conf.default_layout = value;
    } else {
      // Unknown key - store as simple tag
      site_tags[key] = value;
    }
  }

  // Normalize directory paths (convert relative to absolute)
  yass_conf.layout_directory = normalize_path(yass_conf.layout_directory, site_directory);
  yass_conf.output_directory = normalize_path(yass_conf.output_directory, site_directory);
  yass_conf.modules_directory = normalize_path(yass_conf.modules_directory, site_directory);
  yass_conf.content_directory = normalize_path(yass_conf.content_directory, site_directory);

  // Populate excluded files list
  excluded_files.emplace_back(".");
  excluded_files.emplace_back("..");
  excluded_files.emplace_back("site.cfg");

  // Add basename of directories to excluded files
  excluded_files.emplace_back(fs::path(yass_conf.layout_directory).filename().string());
  excluded_files.emplace_back(fs::path(yass_conf.output_directory).filename().string());
  excluded_files.emplace_back(fs::path(yass_conf.modules_directory).filename().string());
}

}  // namespace yass::config
