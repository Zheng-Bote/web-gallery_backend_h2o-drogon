/**
 * SPDX-FileComment: Auth Middleware for Drogon
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file auth_middleware.hpp
 */

#pragma once

#include <drogon/HttpMiddleware.h>
#include "domain/services/token_service.hpp"
#include "core/config/config_loader.hpp"

namespace api::middleware {

class AuthMiddleware : public drogon::HttpMiddleware<AuthMiddleware> {
public:
    void invoke(const drogon::HttpRequestPtr &req,
                drogon::AdviceCallback &&advice,
                drogon::FilterCallback &&next) override {
        
        auto auth_header = req->getHeader("Authorization");
        if (auth_header.empty() || !auth_header.starts_with("Bearer ")) {
            auto resp = drogon::HttpResponse::newHttpStatusCodeResponse(drogon::k401Unauthorized);
            advice(resp);
            return;
        }

        std::string token = auth_header.substr(7);
        auto secret = core::config::ConfigLoader::get("JWT_SECRET");
        
        auto payload = domain::services::TokenService::verify_token(token, secret);
        if (!payload) {
            auto resp = drogon::HttpResponse::newHttpStatusCodeResponse(drogon::k401Unauthorized);
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
