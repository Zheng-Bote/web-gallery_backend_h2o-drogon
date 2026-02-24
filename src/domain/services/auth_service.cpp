/**
 * SPDX-FileComment: Auth Service Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 */

#include "auth_service.hpp"

namespace domain::services {

std::expected<User, std::string> AuthService::authenticate_step1(std::string_view username, std::string_view password) {
    auto user_res = user_repo_->find_by_username(username);
    if (!user_res) return std::unexpected(user_res.error());
    
    auto user_opt = user_res.value();
    if (!user_opt) {
        SecurityEventService::login_failed(username, "User not found");
        return std::unexpected("Invalid credentials");
    }

    auto user = user_opt.value();
    if (!PasswordService::verify_password(password, user.password_hash)) {
        SecurityEventService::login_failed(username, "Wrong password");
        return std::unexpected("Invalid credentials");
    }

    if (!user.is_active) {
        SecurityEventService::login_failed(username, "Account inactive");
        return std::unexpected("Account is locked");
    }

    return user;
}

std::expected<AuthService::LoginResult, std::string> AuthService::authenticate_step2(const User& user, std::string_view totp_code, std::string_view jwt_secret) {
    if (user.totp_secret) {
        if (!TotpService::validate_code(*user.totp_secret, totp_code)) {
            SecurityEventService::totp_failed(user.id, "Invalid code");
            return std::unexpected("Invalid TOTP code");
        }
    }

    TokenPayload payload;
    payload.user_id = user.id;
    payload.username = user.username;
    // roles would be loaded from user.roles

    auto token_res = TokenService::create_token(payload, jwt_secret);
    if (!token_res) return std::unexpected(token_res.error());

    SecurityEventService::login_success(user.id);

    return LoginResult{ .token = token_res.value(), .user = user };
}

} // namespace domain::services
