/**
 * SPDX-FileComment: Auth Controller Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 */

#include "auth_controller.hpp"
#include "domain/services/auth_service.hpp"
#include "infra/repositories/user_repository.hpp"
#include "core/config/config_loader.hpp"
#include <nlohmann/json.hpp>

namespace api::controllers {

void AuthController::login(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("username") || !json->isMember("password")) {
        auto resp = drogon::HttpResponse::newHttpStatusCodeResponse(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    auto user_repo = std::make_shared<infra::repositories::PostgresUserRepository>();
    domain::services::AuthService auth_service(user_repo);

    auto res = auth_service.authenticate_step1((*json)["username"].asString(), 
                                              (*json)["password"].asString());

    if (!res) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(nlohmann::json{{"error", res.error()}});
        resp->setStatusCode(drogon::k401Unauthorized);
        callback(resp);
        return;
    }

    nlohmann::json response;
    if (res.value().totp_secret) {
        response["requires_totp"] = true;
        response["user_id"] = res.value().id;
    } else {
        // Direct login if no TOTP
        auto secret = core::config::ConfigLoader::get("JWT_SECRET");
        auto login_res = auth_service.authenticate_step2(res.value(), "", secret);
        response["token"] = login_res.value().token;
    }

    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

void AuthController::login_totp(const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("user_id") || !json->isMember("code")) {
        auto resp = drogon::HttpResponse::newHttpStatusCodeResponse(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    auto user_repo = std::make_shared<infra::repositories::PostgresUserRepository>();
    auto user_res = user_repo->find_by_id((*json)["user_id"].asString());

    if (!user_res || !user_res.value()) {
        auto resp = drogon::HttpResponse::newHttpStatusCodeResponse(drogon::k401Unauthorized);
        callback(resp);
        return;
    }

    domain::services::AuthService auth_service(user_repo);
    auto secret = core::config::ConfigLoader::get("JWT_SECRET");
    auto res = auth_service.authenticate_step2(user_res.value().value(), 
                                              (*json)["code"].asString(), 
                                              secret);

    if (!res) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(nlohmann::json{{"error", res.error()}});
        resp->setStatusCode(drogon::k401Unauthorized);
        callback(resp);
        return;
    }

    auto resp = drogon::HttpResponse::newHttpJsonResponse(nlohmann::json{{"token", res.value().token}});
    callback(resp);
}

} // namespace api::controllers
