/**
 * SPDX-FileComment: Photo Controller Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file photo_controller.cpp
 * @brief Photo Controller Implementation file
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "photo_controller.hpp"
#include "infra/repositories/photo_repository.hpp"
#include <nlohmann/json.hpp>

namespace api::controllers {

void PhotoController::get_photos(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
  infra::repositories::PostgresPhotoRepository repo;

  int limit = req->getOptionalParameter<int>("limit").value_or(20);
  int offset = req->getOptionalParameter<int>("offset").value_or(0);

  auto result = repo.find_all({.offset = offset, .limit = limit});

  if (!result) {
    auto resp = drogon::HttpResponse::newHttpJsonResponse(
        nlohmann::json{{"error", result.error()}});
    resp->setStatusCode(drogon::k500InternalServerError);
    callback(resp);
    return;
  }

  nlohmann::json j = nlohmann::json::array();
  for (const auto &p : result.value()) {
    j.push_back({{"id", p.id},
                 {"file_name", p.file_name},
                 {"thumb_path", p.thumb_path.value_or("")},
                 {"width", p.width.value_or(0)},
                 {"height", p.height.value_or(0)}});
  }

  auto resp = drogon::HttpResponse::newHttpJsonResponse(j);
  callback(resp);
}

void PhotoController::get_photo_detail(
    const drogon::HttpRequestPtr & /*req*/,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
    std::string id) {
  infra::repositories::PostgresPhotoRepository repo;
  auto result = repo.find_by_id(id);

  if (!result) {
    auto resp = drogon::HttpResponse::newHttpJsonResponse(
        nlohmann::json{{"error", result.error()}});
    resp->setStatusCode(drogon::k500InternalServerError);
    callback(resp);
    return;
  }

  if (!result.value()) {
    auto resp =
        drogon::HttpResponse::newHttpStatusCodeResponse(drogon::k404NotFound);
    callback(resp);
    return;
  }

  const auto &p = result.value().value();
  nlohmann::json j = {{"id", p.id},
                      {"file_name", p.file_name},
                      {"camera_make", p.camera_make.value_or("")},
                      {"camera_model", p.camera_model.value_or("")}};

  auto resp = drogon::HttpResponse::newHttpJsonResponse(j);
  callback(resp);
}

void PhotoController::ping(
    const drogon::HttpRequestPtr & /*req*/,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
  nlohmann::json j = {{"status", "alive"}, {"version", "0.1.0"}};
  auto resp = drogon::HttpResponse::newHttpJsonResponse(j);
  callback(resp);
}

} // namespace api::controllers
