/**
 * SPDX-FileComment: Domain models for User and RBAC
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file user_models.hpp
 * @brief Domain models for User, Roles, and Permissions
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

/**
 * @namespace domain::models
 * @brief Namespace for domain models.
 */
namespace domain::models {

/**
 * @struct Permission
 * @brief Represents a specific action permission.
 */
struct Permission {
  std::string id;
  std::string name;
  std::string description;
};

/**
 * @struct Role
 * @brief Represents a role composed of multiple permissions.
 */
struct Role {
  std::string id;
  std::string name;
  std::string description;
  std::vector<Permission> permissions;
};

/**
 * @struct User
 * @brief Represents a user.
 */
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

/**
 * @struct CommunicationChannel
 * @brief Represents a communication channel belonging to a user.
 */
struct CommunicationChannel {
  std::string id;
  std::string user_id;
  std::string channel_type;
  bool enabled;
  std::string address;
  std::chrono::system_clock::time_point created_at;
};

} // namespace domain::models
