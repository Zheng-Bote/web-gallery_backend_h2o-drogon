/**
 * SPDX-FileComment: Optional Auth Middleware for Drogon
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file optional_auth_middleware.hpp
 * @brief Optional Auth Middleware for Drogon
 * @version 0.1.6
 * @date 2026-02-25
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
#include <drogon/HttpResponse.h>
#include <functional>

/**
 * @namespace api::middleware
 * @brief Namespace for API middleware.
 */
namespace api::middleware {

/**
 * @class OptionalAuthMiddleware
 * @brief Middleware for handling authentication headers optionally.
 */
class OptionalAuthMiddleware : public drogon::HttpMiddleware<OptionalAuthMiddleware> {
public:
  /**
   * @brief Invokes the middleware for the current request.
   */
  void invoke(const drogon::HttpRequestPtr &req,
              std::function<void(const drogon::HttpResponsePtr &)> &&advice,
              std::function<void()> &&next) {
    (void)advice; // Prevent unused parameter warning

    // Default to not authenticated - always insert to avoid exceptions in controller
    req->attributes()->insert("is_authenticated", false);

    auto auth_header = req->getHeader("Authorization");
    if (auth_header.empty() || !auth_header.starts_with("Bearer ")) {
      next();
      return;
    }

    std::string token = auth_header.substr(7);
    auto secret = core::config::ConfigLoader::get("JWT_SECRET");

    auto payload = domain::services::TokenService::verify_token(token, secret);
    if (!payload) {
      next();
      return;
    }

    // Mark as authenticated and store payload info
    req->attributes()->insert("user_id", payload->user_id);
    req->attributes()->insert("username", payload->username);
    req->attributes()->insert("roles", payload->roles);
    req->attributes()->insert("is_authenticated", true);

    next();
  }
};

} // namespace api::middleware
