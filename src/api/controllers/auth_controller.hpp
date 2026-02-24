/**
 * SPDX-FileComment: Auth Controller Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file auth_controller.hpp
 * @brief Auth Controller Header file
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <drogon/HttpController.h>

/**
 * @namespace api::controllers
 * @brief Namespace for API controllers.
 */
namespace api::controllers {

/**
 * @class AuthController
 * @brief Controller for handling authentication requests.
 */
class AuthController : public drogon::HttpController<AuthController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AuthController::login, "/api/login", drogon::Post);
  ADD_METHOD_TO(AuthController::login_totp, "/api/login/totp", drogon::Post);
  METHOD_LIST_END

  /**
   * @brief Handles user login (step 1).
   *
   * @param req The HTTP request.
   * @param callback The response callback.
   */
  void login(const drogon::HttpRequestPtr &req,
             std::function<void(const drogon::HttpResponsePtr &)> &&callback);

  /**
   * @brief Handles TOTP verification for login (step 2).
   *
   * @param req The HTTP request.
   * @param callback The response callback.
   */
  void
  login_totp(const drogon::HttpRequestPtr &req,
             std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace api::controllers
