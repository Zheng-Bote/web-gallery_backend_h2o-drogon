/**
 * SPDX-FileComment: Path Parser for Geo-Hierarchy
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file path_parser.hpp
 */

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <regex>

namespace infra::util {

struct GeoInfo {
    std::optional<std::string> continent;
    std::optional<std::string> country;
    std::optional<std::string> province;
    std::optional<std::string> city;
    std::optional<std::string> fallback_date;
};

class PathParser {
public:
    static GeoInfo parse(const std::filesystem::path& base_path, const std::filesystem::path& file_path) {
        auto relative = std::filesystem::relative(file_path.parent_path(), base_path);
        std::vector<std::string> components;
        for (const auto& part : relative) {
            components.push_back(part.string());
        }

        GeoInfo info;
        if (components.empty()) return info;

        // Date detection (YYYY, YYYY-MM, YYYY-MM-DD)
        std::regex date_regex(R"(^(\d{4})(-\d{2})?(-\d{2})?$)");
        bool last_is_date = std::regex_match(components.back(), date_regex);

        if (last_is_date) {
            info.fallback_date = components.back();
            components.pop_back();
        }

        // Map remaining components to hierarchy
        if (components.size() >= 1) info.continent = components[0];
        if (components.size() >= 2) info.country = components[1];
        if (components.size() >= 3) info.province = components[2];
        if (components.size() >= 4) info.city = components[3];

        return info;
    }
};

} // namespace infra::util
