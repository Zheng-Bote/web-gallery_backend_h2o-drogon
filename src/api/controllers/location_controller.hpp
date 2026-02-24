/**
 * SPDX-FileComment: Location Controller Header
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file location_controller.hpp
 * @brief Location Controller Header file
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
 * @class LocationController
 * @brief Controller for handling location-related requests.
 */
class LocationController : public drogon::HttpController<LocationController> {
public:
  METHOD_LIST_BEGIN
  // Temporär OHNE Auth für den Test im Browser
  ADD_METHOD_TO(LocationController::get_tree, "/api/locations/tree",
                drogon::Get);
  METHOD_LIST_END

  /**
   * @brief Retrieves the location tree.
   *
   * @param req The HTTP request.
   * @param callback The response callback.
   */
  void
  get_tree(const drogon::HttpRequestPtr &req,
           std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace api::controllers
