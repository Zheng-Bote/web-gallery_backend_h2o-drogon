/**
 * SPDX-FileComment: Logging factory using spdlog
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file logger_factory.hpp
 * @brief Factory for creating and managing application and security loggers
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace core::logging {

/**
 * @class LoggerFactory
 * @brief Manages spdlog instances for different parts of the application.
 */
class LoggerFactory {
public:
  /**
   * @brief Initializes the logging system.
   * @param log_path Path to the log file.
   * @param log_level spdlog level (trace, debug, info, warn, error).
   * @param app_env Application environment (development, production).
   */
  static void init(const std::string &log_path, const std::string &log_level,
                   const std::string &app_env);

  /**
   * @brief Gets the main application logger.
   */
  static std::shared_ptr<spdlog::logger> app();

  /**
   * @brief Gets the security audit logger.
   */
  static std::shared_ptr<spdlog::logger> security();

private:
  static inline std::shared_ptr<spdlog::logger> app_logger_;
  static inline std::shared_ptr<spdlog::logger> security_logger_;
};

} // namespace core::logging
