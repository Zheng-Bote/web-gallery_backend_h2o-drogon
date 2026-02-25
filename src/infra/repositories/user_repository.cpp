/**
 * SPDX-FileComment: User Repository Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file user_repository.cpp
 * @brief PostgreSQL Implementation of User Repository
 * @version 0.1.3
 * @date 2026-02-25
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "user_repository.hpp"
#include <drogon/drogon.h>

namespace infra::repositories {

std::expected<std::optional<domain::models::User>, std::string>
PostgresUserRepository::find_by_username(std::string_view username) {
  auto db = drogon::app().getDbClient();
  try {
    auto result = db->execSqlSync("SELECT * FROM users WHERE username = $1",
                                  std::string(username));
    if (result.empty())
      return std::nullopt;

    const auto &row = result[0];
    domain::models::User u;
    u.id = row["id"].template as<std::string>();
    u.username = row["username"].template as<std::string>();
    u.password_hash = row["password_hash"].template as<std::string>();
    if (!row["totp_secret"].isNull())
      u.totp_secret = row["totp_secret"].template as<std::string>();
    u.is_active = row["is_active"].template as<bool>();
    u.pwd_must_change = row["pwd_must_change"].template as<bool>();
    u.language = row["language"].template as<std::string>();

    return u;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<std::optional<domain::models::User>, std::string>
PostgresUserRepository::find_by_id(std::string_view id) {
  auto db = drogon::app().getDbClient();
  try {
    auto result =
        db->execSqlSync("SELECT * FROM users WHERE id = $1", std::string(id));
    if (result.empty())
      return std::nullopt;

    const auto &row = result[0];
    domain::models::User u;
    u.id = row["id"].template as<std::string>();
    u.username = row["username"].template as<std::string>();
    // ... map other fields
    return u;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<void, std::string>
PostgresUserRepository::save(const domain::models::User &user) {
  auto db = drogon::app().getDbClient();
  try {
    db->execSqlSync("INSERT INTO users (id, username, password_hash, "
                    "totp_secret, is_active, pwd_must_change, language) "
                    "VALUES ($1, $2, $3, $4, $5, $6, $7) "
                    "ON CONFLICT (id) DO UPDATE SET password_hash = "
                    "EXCLUDED.password_hash, is_active = EXCLUDED.is_active",
                    user.id, user.username, user.password_hash,
                    user.totp_secret.value_or(""), user.is_active,
                    user.pwd_must_change, user.language);
    return {};
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<std::vector<domain::models::Permission>, std::string>
PostgresUserRepository::get_user_permissions(std::string_view user_id) {
  auto db = drogon::app().getDbClient();
  try {
    auto result =
        db->execSqlSync("SELECT p.id, p.name, p.description FROM permissions p "
                        "JOIN role_permissions rp ON p.id = rp.permission_id "
                        "JOIN user_roles ur ON rp.role_id = ur.role_id "
                        "WHERE ur.user_id = $1",
                        std::string(user_id));

    std::vector<domain::models::Permission> perms;
    for (const auto &row : result) {
      domain::models::Permission p;
      p.id = row["id"].template as<std::string>();
      p.name = row["name"].template as<std::string>();
      p.description = row["description"].template as<std::string>();
      perms.push_back(p);
    }
    return perms;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<std::vector<domain::models::CommunicationChannel>, std::string>
PostgresUserRepository::get_user_channels(std::string_view user_id) {
  auto db = drogon::app().getDbClient();
  try {
    auto result = db->execSqlSync(
        "SELECT * FROM communication_channels WHERE user_id = $1",
        std::string(user_id));
    std::vector<domain::models::CommunicationChannel> channels;
    for (const auto &row : result) {
      domain::models::CommunicationChannel c;
      c.id = row["id"].template as<std::string>();
      c.user_id = row["user_id"].template as<std::string>();
      c.channel_type = row["channel_type"].template as<std::string>();
      c.enabled = row["enabled"].template as<bool>();
      c.address = row["address"].template as<std::string>();
      c.created_at = std::chrono::system_clock::now();
      channels.push_back(c);
    }
    return channels;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

} // namespace infra::repositories
