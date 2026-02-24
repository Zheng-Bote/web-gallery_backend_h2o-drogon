/**
 * SPDX-FileComment: Photo API Controller
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file photo_controller.hpp
 */

#pragma once

#include <drogon/HttpController.h>
#include "api/middleware/auth_middleware.hpp"

namespace api::controllers {

class PhotoController : public drogon::HttpController<PhotoController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(PhotoController::get_photos, "/api/photos", drogon::Get, "api::middleware::AuthMiddleware");
        ADD_METHOD_TO(PhotoController::get_photo_detail, "/api/photos/{id}", drogon::Get, "api::middleware::AuthMiddleware");
        ADD_METHOD_TO(PhotoController::ping, "/api/ping", drogon::Get);
    METHOD_LIST_END

    void get_photos(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void get_photo_detail(const drogon::HttpRequestPtr &req,
                         std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                         std::string id);

    void ping(const drogon::HttpRequestPtr &req,
              std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace api::controllers
