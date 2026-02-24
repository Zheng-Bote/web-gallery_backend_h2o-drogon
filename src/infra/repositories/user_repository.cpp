/**
 * SPDX-FileComment: User Repository Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 */

#include "user_repository.hpp"
#include <drogon/drogon.h>

namespace infra::repositories {

std::expected<std::optional<User>, std::string> PostgresUserRepository::find_by_username(std::string_view username) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync("SELECT * FROM users WHERE username = $1", std::string(username));
        if (result.empty()) return std::nullopt;

        const auto& row = result[0];
        User u;
        u.id = row["id"].as<std::string>();
        u.username = row["username"].as<std::string>();
        u.password_hash = row["password_hash"].as<std::string>();
        if (!row["totp_secret"].isNull()) u.totp_secret = row["totp_secret"].as<std::string>();
        u.is_active = row["is_active"].as<bool>();
        u.pwd_must_change = row["pwd_must_change"].as<bool>();
        u.language = row["language"].as<std::string>();
        
        return u;
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

std::expected<std::optional<User>, std::string> PostgresUserRepository::find_by_id(std::string_view id) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync("SELECT * FROM users WHERE id = $1", std::string(id));
        if (result.empty()) return std::nullopt;

        const auto& row = result[0];
        User u;
        u.id = row["id"].as<std::string>();
        u.username = row["username"].as<std::string>();
        // ... map other fields
        return u;
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

std::expected<void, std::string> PostgresUserRepository::save(const User& user) {
    auto db = drogon::app().getDbClient();
    try {
        db->execSqlSync("INSERT INTO users (id, username, password_hash, totp_secret, is_active, pwd_must_change, language) "
                       "VALUES ($1, $2, $3, $4, $5, $6, $7) "
                       "ON CONFLICT (id) DO UPDATE SET password_hash = EXCLUDED.password_hash, is_active = EXCLUDED.is_active",
                       user.id, user.username, user.password_hash, user.totp_secret.value_or(""), 
                       user.is_active, user.pwd_must_change, user.language);
        return {};
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

std::expected<std::vector<Permission>, std::string> PostgresUserRepository::get_user_permissions(std::string_view user_id) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync(
            "SELECT p.name, p.description FROM permissions p "
            "JOIN role_permissions rp ON p.id = rp.permission_id "
            "JOIN user_roles ur ON rp.role_id = ur.role_id "
            "WHERE ur.user_id = $1", std::string(user_id));
        
        std::vector<Permission> perms;
        for (const auto& row : result) {
            perms.push_back({ .name = row["name"].as<std::string>(), .description = row["description"].as<std::string>() });
        }
        return perms;
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

std::expected<std::vector<CommunicationChannel>, std::string> PostgresUserRepository::get_user_channels(std::string_view user_id) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync("SELECT * FROM communication_channels WHERE user_id = $1", std::string(user_id));
        std::vector<CommunicationChannel> channels;
        for (const auto& row : result) {
            channels.push_back({
                .id = row["id"].as<std::string>(),
                .user_id = row["user_id"].as<std::string>(),
                .channel_type = row["channel_type"].as<std::string>(),
                .enabled = row["enabled"].as<bool>(),
                .address = row["address"].as<std::string>()
            });
        }
        return channels;
    } catch (const std::exception& e) {
        return std::unexpected(e.what());
    }
}

} // namespace infra::repositories
