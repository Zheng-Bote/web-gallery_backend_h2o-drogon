/**
 * SPDX-FileComment: Security Event Service
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file security_event_service.hpp
 * @brief Service for logging security-relevant audit events
 */

#pragma once

#include "core/logging/logger_factory.hpp"
#include <string>
#include <string_view>
#include <optional>
#include <format>

namespace domain::services {

/**
 * @class SecurityEventService
 * @brief Handles logging of security events such as login failures, role changes, etc.
 */
class SecurityEventService {
public:
    static void log_event(std::string_view event_type, 
                         std::optional<std::string_view> user_id, 
                         std::string_view details) {
        std::string log_msg = std::format("user={} event={} details="{}"", 
                                          user_id.value_or("unknown"), 
                                          event_type, 
                                          details);
        core::logging::LoggerFactory::security()->info(log_msg);
    }

    static void login_success(std::string_view user_id) {
        log_event("login_success", user_id, "User successfully logged in");
    }

    static void login_failed(std::string_view username, std::string_view reason) {
        log_event("login_failed", std::nullopt, std::format("username={} reason="{}"", username, reason));
    }

    static void password_changed(std::string_view user_id) {
        log_event("password_changed", user_id, "User changed their password");
    }

    static void totp_failed(std::string_view user_id, std::string_view reason) {
        log_event("totp_validation_failed", user_id, reason);
    }
};

} // namespace domain::services
