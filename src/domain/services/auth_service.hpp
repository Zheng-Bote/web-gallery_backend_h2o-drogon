/**
 * SPDX-FileComment: Auth Service
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file auth_service.hpp
 */

#pragma once

#include "domain/interfaces/i_user_repository.hpp"
#include "domain/services/password_service.hpp"
#include "domain/services/token_service.hpp"
#include "domain/services/totp_service.hpp"
#include "domain/services/security_event_service.hpp"
#include <memory>

namespace domain::services {

class AuthService {
public:
    AuthService(std::shared_ptr<interfaces::IUserRepository> user_repo)
        : user_repo_(std::move(user_repo)) {}

    struct LoginResult {
        std::string token;
        User user;
    };

    std::expected<User, std::string> authenticate_step1(std::string_view username, std::string_view password);
    std::expected<LoginResult, std::string> authenticate_step2(const User& user, std::string_view totp_code, std::string_view jwt_secret);

private:
    std::shared_ptr<interfaces::IUserRepository> user_repo_;
};

} // namespace domain::services
