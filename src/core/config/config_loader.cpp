/**
 * SPDX-FileComment: Configuration loader implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file config_loader.cpp
 * @brief Implementation of .env file parsing and environment variable management
 */

#include "config_loader.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <print>
#include <ranges>
#include <algorithm>

namespace core::config {

std::expected<void, std::string> ConfigLoader::load(std::string_view path) {
    std::ifstream file{std::string(path)};
    if (!file.is_open()) {
        return std::unexpected(std::format("Could not open config file: {}", path));
    }

    std::string line;
    while (std::getline(file, line)) {
        // Remove comments
        if (auto pos = line.find('#'); pos != std::string::npos) {
            line.erase(pos);
        }

        // Trim whitespace
        auto is_not_space = [](unsigned char ch) { return !std::isspace(ch); };
        auto start = std::ranges::find_if(line, is_not_space);
        auto end = std::ranges::find_if(line | std::views::reverse, is_not_space).base();
        
        if (start == line.end()) continue;
        std::string trimmed(start, end);

        if (auto delimiter_pos = trimmed.find('='); delimiter_pos != std::string::npos) {
            std::string key = trimmed.substr(0, delimiter_pos);
            std::string value = trimmed.substr(delimiter_pos + 1);

            // Clean key/value
            auto clean = [](std::string& s) {
                s.erase(std::remove(s.begin(), s.end(), '"'), s.end());
                s.erase(std::remove(s.begin(), s.end(), '\''), s.end());
            };
            clean(key);
            clean(value);

            if (!key.empty()) {
                cache_[key] = value;
                setenv(key.c_str(), value.c_str(), 1);
            }
        }
    }
    return {};
}

std::string ConfigLoader::get(std::string_view key, std::string_view default_value) {
    if (const char* env_p = std::getenv(key.data())) {
        return std::string(env_p);
    }
    auto it = cache_.find(std::string(key));
    if (it != cache_.end()) {
        return it->second;
    }
    return std::string(default_value);
}

std::expected<std::string, std::string> ConfigLoader::get_required(std::string_view key) {
    auto val = get(key);
    if (val.empty()) {
        return std::unexpected(std::format("Missing required configuration: {}", key));
    }
    return val;
}

} // namespace core::config
