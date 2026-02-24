/**
 * SPDX-FileComment: Path Parser for Geo-Hierarchy
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file path_parser.hpp
 * @brief Extracts geo information from file paths
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <filesystem>
#include <optional>
#include <regex>
#include <string>
#include <vector>

/**
 * @namespace infra::util
 * @brief Namespace for utility components.
 */
namespace infra::util {

/**
 * @struct GeoInfo
 * @brief Extracted geographical information from a path.
 */
struct GeoInfo {
  std::optional<std::string> continent;
  std::optional<std::string> country;
  std::optional<std::string> province;
  std::optional<std::string> city;
  std::optional<std::string> fallback_date;
};

/**
 * @class PathParser
 * @brief Parses file paths into geo-hierarchy components.
 */
class PathParser {
public:
  /**
   * @brief Parses a file path relative to a base path.
   * @param base_path The root directory of the photos.
   * @param file_path The absolute path to the photo file.
   * @return GeoInfo The extracted geographic hierarchy.
   */
  static GeoInfo parse(const std::filesystem::path &base_path,
                       const std::filesystem::path &file_path) {
    auto relative =
        std::filesystem::relative(file_path.parent_path(), base_path);
    std::vector<std::string> components;
    for (const auto &part : relative) {
      components.push_back(part.string());
    }

    GeoInfo info;
    if (components.empty())
      return info;

    // Date detection (YYYY, YYYY-MM, YYYY-MM-DD)
    std::regex date_regex(R"(^(\d{4})(-\d{2})?(-\d{2})?$)");
    bool last_is_date = std::regex_match(components.back(), date_regex);

    if (last_is_date) {
      info.fallback_date = components.back();
      components.pop_back();
    }

    // Map remaining components to hierarchy
    if (components.size() >= 1)
      info.continent = components[0];
    if (components.size() >= 2)
      info.country = components[1];
    if (components.size() >= 3)
      info.province = components[2];
    if (components.size() >= 4)
      info.city = components[3];

    return info;
  }
};

} // namespace infra::util
