/**
 * SPDX-FileComment: Auth Service
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file auth_service.hpp
 * @brief Auth Service
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "domain/interfaces/i_user_repository.hpp"
#include "domain/models/user_models.hpp"
#include "domain/services/password_service.hpp"
#include "domain/services/security_event_service.hpp"
#include "domain/services/token_service.hpp"
#include "domain/services/totp_service.hpp"
#include <memory>

/**
 * @namespace domain::services
 * @brief Namespace for domain services.
 */
namespace domain::services {

/**
 * @class AuthService
 * @brief Service containing authentication business logic.
 */
class AuthService {
public:
  /**
   * @brief Constructor for AuthService.
   * @param user_repo Shared pointer to the user repository interface.
   */
  AuthService(std::shared_ptr<interfaces::IUserRepository> user_repo)
      : user_repo_(std::move(user_repo)) {}

  /**
   * @struct LoginResult
   * @brief Data returned after successful login.
   */
  struct LoginResult {
    std::string token;
    domain::models::User user;
  };

  /**
   * @brief Authenticates a user's password (Step 1).
   * @param username The user's username.
   * @param password The user's plaintext password.
   * @return std::expected<domain::models::User, std::string> The authenticated User, or an
   * error.
   */
  std::expected<domain::models::User, std::string>
  authenticate_step1(std::string_view username, std::string_view password);

  /**
   * @brief Validates a user's TOTP code and generates a login token (Step 2).
   * @param user The authenticated User.
   * @param totp_code The TOTP code provided by the user.
   * @param jwt_secret The JWT signing secret.
   * @return std::expected<LoginResult, std::string> The login result containing
   * the token, or an error.
   */
  std::expected<LoginResult, std::string>
  authenticate_step2(const domain::models::User &user, std::string_view totp_code,
                     std::string_view jwt_secret);

private:
  std::shared_ptr<interfaces::IUserRepository> user_repo_;
};

} // namespace domain::services
