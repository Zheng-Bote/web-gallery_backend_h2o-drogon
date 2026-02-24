/**
 * SPDX-FileComment: Auth Middleware for Drogon
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file auth_middleware.hpp
 * @brief Auth Middleware for Drogon
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "core/config/config_loader.hpp"
#include "domain/services/token_service.hpp"
#include <drogon/HttpMiddleware.h>

/**
 * @namespace api::middleware
 * @brief Namespace for API middleware.
 */
namespace api::middleware {

/**
 * @class AuthMiddleware
 * @brief Middleware for handling authentication headers.
 */
class AuthMiddleware : public drogon::HttpMiddleware<AuthMiddleware> {
public:
  /**
   * @brief Invokes the middleware for the current request.
   *
   * @param req The HTTP request.
   * @param advice Callback for returning early with a response.
   * @param next Callback to continue processing the request.
   */
  void invoke(const drogon::HttpRequestPtr &req,
              drogon::AdviceCallback &&advice,
              drogon::FilterCallback &&next) override {

    auto auth_header = req->getHeader("Authorization");
    if (auth_header.empty() || !auth_header.starts_with("Bearer ")) {
      auto resp = drogon::HttpResponse::newHttpStatusCodeResponse(
          drogon::k401Unauthorized);
      advice(resp);
      return;
    }

    std::string token = auth_header.substr(7);
    auto secret = core::config::ConfigLoader::get("JWT_SECRET");

    auto payload = domain::services::TokenService::verify_token(token, secret);
    if (!payload) {
      auto resp = drogon::HttpResponse::newHttpStatusCodeResponse(
          drogon::k401Unauthorized);
      advice(resp);
      return;
    }

    // Store user info in request context
    req->attributes()->insert("user_id", payload->user_id);
    req->attributes()->insert("username", payload->username);
    req->attributes()->insert("roles", payload->roles);

    next();
  }
};

} // namespace api::middleware
