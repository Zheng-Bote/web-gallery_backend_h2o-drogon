/**
 * SPDX-FileComment: User Controller Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 */

#include "user_controller.hpp"
#include "infra/repositories/user_repository.hpp"
#include <nlohmann/json.hpp>

namespace api::controllers {

void UserController::get_me(const drogon::HttpRequestPtr &req,
                           std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    auto user_id = req->attributes()->get<std::string>("user_id");
    
    infra::repositories::PostgresUserRepository repo;
    auto perms = repo.get_user_permissions(user_id);
    auto channels = repo.get_user_channels(user_id);

    nlohmann::json j;
    j["id"] = user_id;
    j["username"] = req->attributes()->get<std::string>("username");
    
    j["permissions"] = nlohmann::json::array();
    if (perms) {
        for (const auto& p : perms.value()) j["permissions"].push_back(p.name);
    }

    j["channels"] = nlohmann::json::array();
    if (channels) {
        for (const auto& c : channels.value()) {
            j["channels"].push_back({
                {"type", c.channel_type},
                {"address", c.address},
                {"enabled", c.enabled}
            });
        }
    }

    auto resp = drogon::HttpResponse::newHttpJsonResponse(j);
    callback(resp);
}

} // namespace api::controllers
