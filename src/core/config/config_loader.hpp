/**
 * SPDX-FileComment: Configuration loader for .env files
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file config_loader.hpp
 * @brief Handles loading and retrieving environment variables from .env files
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 */

#pragma once

#include <string>
#include <string_view>
#include <expected>
#include <map>

namespace core::config {

/**
 * @class ConfigLoader
 * @brief Singleton class to manage application configuration via environment variables.
 */
class ConfigLoader {
public:
    /**
     * @brief Loads the .env file from the specified path.
     * @param path Path to the .env file.
     * @return std::expected<void, std::string> Success or error message.
     */
    static std::expected<void, std::string> load(std::string_view path = ".env");

    /**
     * @brief Retrieves a configuration value by key.
     * @param key The environment variable key.
     * @param default_value Optional default value if key is not found.
     * @return std::string The value of the key or default_value.
     */
    static std::string get(std::string_view key, std::string_view default_value = "");

    /**
     * @brief Retrieves a required configuration value.
     * @param key The environment variable key.
     * @return std::expected<std::string, std::string> The value or an error if missing.
     */
    static std::expected<std::string, std::string> get_required(std::string_view key);

private:
    ConfigLoader() = default;
    static inline std::map<std::string, std::string> cache_;
};

} // namespace core::config
