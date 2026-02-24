/**
 * SPDX-FileComment: User Controller Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <drogon/HttpController.h>

namespace api::controllers {

class UserController : public drogon::HttpController<UserController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UserController::get_me, "/api/users/me", drogon::Get, "api::middleware::AuthMiddleware");
    METHOD_LIST_END

    void get_me(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace api::controllers
