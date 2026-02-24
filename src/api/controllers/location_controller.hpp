/**
 * SPDX-FileComment: Location Controller Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <drogon/HttpController.h>

namespace api::controllers {

class LocationController : public drogon::HttpController<LocationController> {
public:
    METHOD_LIST_BEGIN
        // Temporär OHNE Auth für den Test im Browser
        ADD_METHOD_TO(LocationController::get_tree, "/api/locations/tree", drogon::Get);
    METHOD_LIST_END

    void get_tree(const drogon::HttpRequestPtr &req,
                  std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace api::controllers
