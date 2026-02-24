/**
 * SPDX-FileComment: User Controller Header
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file user_controller.hpp
 * @brief User Controller Header file
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
 * @class UserController
 * @brief Controller for handling user-related requests.
 */
class UserController : public drogon::HttpController<UserController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(UserController::get_me, "/api/users/me", drogon::Get,
                "api::middleware::AuthMiddleware");
  METHOD_LIST_END

  /**
   * @brief Retrieves the generalized profile of the authorized user.
   *
   * @param req The HTTP request.
   * @param callback The response callback.
   */
  void get_me(const drogon::HttpRequestPtr &req,
              std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace api::controllers
