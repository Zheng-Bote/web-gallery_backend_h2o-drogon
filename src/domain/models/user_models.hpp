/**
 * SPDX-FileComment: Domain models for User and RBAC
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file user_models.hpp
 */

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <optional>

namespace domain::models {

struct Permission {
    std::string id;
    std::string name;
    std::string description;
};

struct Role {
    std::string id;
    std::string name;
    std::string description;
    std::vector<Permission> permissions;
};

struct User {
    std::string id;
    std::string username;
    std::string password_hash;
    std::optional<std::string> totp_secret;
    bool is_active;
    bool pwd_must_change;
    std::string language;
    std::optional<std::chrono::system_clock::time_point> last_login_at;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
    std::vector<Role> roles;
};

struct CommunicationChannel {
    std::string id;
    std::string user_id;
    std::string channel_type;
    bool enabled;
    std::string address;
    std::chrono::system_clock::time_point created_at;
};

} // namespace domain::models
