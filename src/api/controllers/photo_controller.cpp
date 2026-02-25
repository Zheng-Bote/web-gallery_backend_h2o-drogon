/**
 * SPDX-FileComment: Photo Controller Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file photo_controller.cpp
 * @brief Photo Controller Implementation file
 * @version 0.1.3
 * @date 2026-02-25
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "photo_controller.hpp"
#include "infra/repositories/photo_repository.hpp"
#include <drogon/HttpResponse.h>
#include <nlohmann/json.hpp>

using namespace domain::interfaces;

namespace api::controllers {

void PhotoController::get_photos(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
  infra::repositories::PostgresPhotoRepository repo;

  int limit = req->getOptionalParameter<int>("limit").value_or(20);
  int offset = req->getOptionalParameter<int>("offset").value_or(0);

  bool is_authenticated = false;
  try {
    is_authenticated = req->attributes()->get<bool>("is_authenticated");
  } catch (...) {}

  PhotoFilter filter;
  filter.offset = offset;
  filter.limit = limit;
  if (!is_authenticated) {
    filter.is_public = true;
  }

  auto result = repo.find_all(filter);

  if (!result) {
    nlohmann::json error_json = {{"error", result.error()}};
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(error_json.dump());
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
    callback(resp);
    return;
  }

  nlohmann::json j = nlohmann::json::array();
  for (const auto &p : result.value()) {
    j.push_back({{"id", p.id},
                 {"file_name", p.file_name},
                 {"thumb_path", p.thumb_path.value_or("")},
                 {"width", p.width.value_or(0)},
                 {"height", p.height.value_or(0)},
                 {"is_public", p.is_public}});
  }

  auto resp = drogon::HttpResponse::newHttpResponse();
  resp->setBody(j.dump());
  resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
  callback(resp);
}

void PhotoController::get_photo_detail(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    std::string id) {
  
  // Handle empty ID (e.g. from /api/photos/) by delegating to get_photos
  if (id.empty()) {
      get_photos(req, std::move(callback));
      return;
  }

  infra::repositories::PostgresPhotoRepository repo;
  auto result = repo.find_by_id(id);

  if (!result) {
    nlohmann::json error_json = {{"error", result.error()}};
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(error_json.dump());
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
    callback(resp);
    return;
  }

  if (!result.value()) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::HttpStatusCode::k404NotFound);
    callback(resp);
    return;
  }

  const auto &p = result.value().value();
  bool is_authenticated = false;
  try {
    is_authenticated = req->attributes()->get<bool>("is_authenticated");
  } catch (...) {}

  if (!is_authenticated && !p.is_public) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
    callback(resp);
    return;
  }

  nlohmann::json j = {{"id", p.id},
                      {"file_name", p.file_name},
                      {"camera_make", p.camera_make.value_or("")},
                      {"camera_model", p.camera_model.value_or("")},
                      {"is_public", p.is_public}};

  auto resp = drogon::HttpResponse::newHttpResponse();
  resp->setBody(j.dump());
  resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
  callback(resp);
}

void PhotoController::ping(
    const drogon::HttpRequestPtr & /*req*/,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
  nlohmann::json j = {{"status", "alive"}, {"version", "0.1.0"}};
  auto resp = drogon::HttpResponse::newHttpResponse();
  resp->setBody(j.dump());
  resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
  callback(resp);
}

} // namespace api::controllers
