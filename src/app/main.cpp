/**
 * SPDX-FileComment: Main application entry point
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file main.cpp
 * @brief Application entry point and server setup
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "core/config/config_loader.hpp"
#include "core/logging/logger_factory.hpp"
#include <drogon/drogon.h>
#include <print>

// Explicitly include controllers to ensure they are linked and registered
#include "api/controllers/auth_controller.hpp"
#include "api/controllers/location_controller.hpp"
#include "api/controllers/photo_controller.hpp"
#include "api/controllers/user_controller.hpp"

using namespace core::config;
using namespace core::logging;

/**
 * @brief Main entry point for the PhotoGallery Backend server.
 *
 * @return int The exit status.
 */
int main() {
  // 1. Load configuration
  if (auto res = ConfigLoader::load(); !res) {
    std::println(stderr, "Failed to load .env: {}", res.error());
    return 1;
  }

  // 2. Initialize Logging
  auto log_path = ConfigLoader::get("LOG_PATH", "logs/backend.log");
  auto log_level = ConfigLoader::get("LOG_LEVEL", "debug");
  auto app_env = ConfigLoader::get("APP_ENV", "development");
  LoggerFactory::init(log_path, log_level, app_env);

  auto logger = LoggerFactory::app();
  logger->info("Starting PhotoGallery Backend in {} mode", app_env);

  // 3. Configure Drogon
  auto port = static_cast<uint16_t>(
      std::stoi(ConfigLoader::get("SERVER_PORT", "8848")));

  Json::Value config;
  Json::Value listener;
  listener["address"] = "0.0.0.0";
  listener["port"] = port;
  config["listeners"].append(listener);
  config["app"]["number_of_threads"] = 0;

  Json::Value cors;
  cors["allow_origin"] = "*";
  cors["allow_methods"] = "GET,POST,PUT,DELETE,OPTIONS";
  cors["allow_headers"] = "Content-Type,Authorization";
  config["app"]["cors"] = cors;

  Json::Value db_client;
  db_client["name"] = "default";
  db_client["rdbms"] = "postgresql";
  db_client["host"] = ConfigLoader::get("DB_HOST", "psql_db");
  db_client["port"] = std::stoi(ConfigLoader::get("DB_PORT", "5432"));
  db_client["dbname"] = ConfigLoader::get("DB_NAME", "gallery");
  db_client["user"] = ConfigLoader::get("DB_USER", "gallery_user");
  db_client["passwd"] = ConfigLoader::get("DB_PASSWORD", "");
  db_client["connection_number"] = 5;
  config["db_clients"].append(db_client);

  drogon::app().loadConfigJson(config);

  logger->info("Server listening on port {}", port);

  // 4. Run server
  drogon::app().run();

  return 0;
}
