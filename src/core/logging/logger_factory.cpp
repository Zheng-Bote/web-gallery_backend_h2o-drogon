/**
 * SPDX-FileComment: Logging factory implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file logger_factory.cpp
 * @brief Implementation of the logging factory using spdlog
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "logger_factory.hpp"
#include <filesystem>
#include <print>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace core::logging {

void LoggerFactory::init(const std::string &log_path,
                         const std::string &log_level,
                         const std::string &app_env) {
  try {
    std::filesystem::path p(log_path);
    if (!p.parent_path().empty()) {
      std::filesystem::create_directories(p.parent_path());
    }

    auto level = spdlog::level::from_str(log_level);

    std::vector<spdlog::sink_ptr> sinks;

    // Console sink (only in development)
    if (app_env == "development") {
      sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    // Rotating file sink
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        log_path, 10 * 1024 * 1024, 5));

    app_logger_ =
        std::make_shared<spdlog::logger>("app", sinks.begin(), sinks.end());
    app_logger_->set_level(level);
    app_logger_->flush_on(spdlog::level::err);
    spdlog::register_logger(app_logger_);

    // Security logger
    std::string sec_log_path = p.parent_path().string() + "/security.log";
    auto sec_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        sec_log_path, 10 * 1024 * 1024, 5);
    security_logger_ = std::make_shared<spdlog::logger>("security", sec_sink);
    security_logger_->set_level(spdlog::level::info);
    security_logger_->set_pattern("[%Y-%m-%dT%H:%M:%SZ] %v");
    security_logger_->flush_on(spdlog::level::info);
    spdlog::register_logger(security_logger_);

    spdlog::set_default_logger(app_logger_);

  } catch (const spdlog::spdlog_ex &ex) {
    std::println(stderr, "Log initialization failed: {}", ex.what());
  }
}

std::shared_ptr<spdlog::logger> LoggerFactory::app() { return app_logger_; }
std::shared_ptr<spdlog::logger> LoggerFactory::security() {
  return security_logger_;
}

} // namespace core::logging
