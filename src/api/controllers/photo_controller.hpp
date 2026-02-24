/**
 * SPDX-FileComment: Photo API Controller
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file photo_controller.hpp
 * @brief Photo API Controller Header file
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "api/middleware/auth_middleware.hpp"
#include <drogon/HttpController.h>

/**
 * @namespace api::controllers
 * @brief Namespace for API controllers.
 */
namespace api::controllers {

/**
 * @class PhotoController
 * @brief Controller for handling photo-related requests.
 */
class PhotoController : public drogon::HttpController<PhotoController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(PhotoController::get_photos, "/api/photos", drogon::Get,
                "api::middleware::AuthMiddleware");
  ADD_METHOD_TO(PhotoController::get_photo_detail, "/api/photos/{id}",
                drogon::Get, "api::middleware::AuthMiddleware");
  ADD_METHOD_TO(PhotoController::ping, "/api/ping", drogon::Get);
  METHOD_LIST_END

  /**
   * @brief Retrieves a list of photos.
   *
   * @param req The HTTP request.
   * @param callback The response callback.
   */
  void
  get_photos(const drogon::HttpRequestPtr &req,
             std::function<void(const drogon::HttpResponsePtr &)> &&callback);

  /**
   * @brief Retrieves details for a specific photo.
   *
   * @param req The HTTP request.
   * @param callback The response callback.
   * @param id The ID of the photo.
   */
  void get_photo_detail(
      const drogon::HttpRequestPtr &req,
      std::function<void(const drogon::HttpResponsePtr &)> &&callback,
      std::string id);

  /**
   * @brief Pings the photo API service.
   *
   * @param req The HTTP request.
   * @param callback The response callback.
   */
  void ping(const drogon::HttpRequestPtr &req,
            std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace api::controllers
