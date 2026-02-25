/**
 * SPDX-FileComment: Auth Controller Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file auth_controller.cpp
 * @brief Auth Controller Implementation file
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "auth_controller.hpp"
#include "core/config/config_loader.hpp"
#include "domain/services/auth_service.hpp"
#include "infra/repositories/user_repository.hpp"
#include <drogon/HttpResponse.h>
#include <nlohmann/json.hpp>

namespace api::controllers {

void AuthController::login(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
  auto json = req->getJsonObject();
  if (!json || !json->isMember("username") || !json->isMember("password")) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
    callback(resp);
    return;
  }

  auto user_repo =
      std::make_shared<infra::repositories::PostgresUserRepository>();
  domain::services::AuthService auth_service(user_repo);

  auto res = auth_service.authenticate_step1((*json)["username"].asString(),
                                             (*json)["password"].asString());

  if (!res) {
    nlohmann::json error_json = {{"error", res.error()}};
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(error_json.dump());
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
    callback(resp);
    return;
  }

  nlohmann::json response;
  if (res.value().totp_secret && !res.value().totp_secret->empty()) {
    response["requires_totp"] = true;
    response["user_id"] = res.value().id;
  } else {
    // Direct login if no TOTP
    auto secret = core::config::ConfigLoader::get("JWT_SECRET");
    auto login_res = auth_service.authenticate_step2(res.value(), "", secret);
    if (login_res) {
      response["token"] = login_res.value().token;
    } else {
      nlohmann::json error_json = {{"error", login_res.error()}};
      auto resp = drogon::HttpResponse::newHttpResponse();
      resp->setBody(error_json.dump());
      resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
      resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
      callback(resp);
      return;
    }
  }

  auto resp = drogon::HttpResponse::newHttpResponse();
  resp->setBody(response.dump());
  resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
  callback(resp);
}

void AuthController::login_totp(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
  auto json = req->getJsonObject();
  if (!json || !json->isMember("user_id") || !json->isMember("code")) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
    callback(resp);
    return;
  }

  auto user_repo =
      std::make_shared<infra::repositories::PostgresUserRepository>();
  auto user_res = user_repo->find_by_id((*json)["user_id"].asString());

  if (!user_res || !user_res.value()) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
    callback(resp);
    return;
  }

  domain::services::AuthService auth_service(user_repo);
  auto secret = core::config::ConfigLoader::get("JWT_SECRET");
  auto res = auth_service.authenticate_step2(
      user_res.value().value(), (*json)["code"].asString(), secret);

  if (!res) {
    nlohmann::json error_json = {{"error", res.error()}};
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(error_json.dump());
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
    callback(resp);
    return;
  }

  nlohmann::json response = {{"token", res.value().token}};
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp->setBody(response.dump());
  resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
  callback(resp);
}

} // namespace api::controllers
