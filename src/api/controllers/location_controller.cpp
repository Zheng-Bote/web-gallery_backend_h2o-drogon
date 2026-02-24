/**
 * SPDX-FileComment: Location Controller Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 */

#include "location_controller.hpp"
#include "infra/repositories/photo_repository.hpp"
#include <nlohmann/json.hpp>
#include <map>

namespace api::controllers {

void LocationController::get_tree(const drogon::HttpRequestPtr &/*req*/,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    infra::repositories::PostgresLocationRepository repo;
    auto res = repo.get_tree();

    if (!res) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(nlohmann::json{{"error", res.error()}});
        resp->setStatusCode(drogon::k500InternalServerError);
        callback(resp);
        return;
    }

    nlohmann::json root;
    auto& continents_arr = root["continents"] = nlohmann::json::array();

    // Map-Struktur zum Gruppieren: Continent -> Country -> Province -> Set of Cities
    std::map<std::string, std::map<std::string, std::map<std::string, std::vector<std::string>>>> tree;

    for (const auto& l : res.value()) {
        std::string cont = l.continent.value_or("Unknown");
        std::string country = l.country.value_or("Unknown");
        std::string prov = l.province.value_or("Unknown");
        std::string city = l.city.value_or("Unknown");
        
        tree[cont][country][prov].push_back(city);
    }

    // JSON-Struktur aufbauen
    for (auto const& [cont_name, countries] : tree) {
        nlohmann::json cont_json = {{"name", cont_name}, {"countries", nlohmann::json::array()}};
        
        for (auto const& [country_name, provinces] : countries) {
            nlohmann::json country_json = {{"name", country_name}, {"provinces", nlohmann::json::array()}};
            
            for (auto const& [prov_name, cities] : provinces) {
                country_json["provinces"].push_back({
                    {"name", prov_name},
                    {"cities", cities}
                });
            }
            cont_json["countries"].push_back(country_json);
        }
        continents_arr.push_back(cont_json);
    }

    auto resp = drogon::HttpResponse::newHttpJsonResponse(root);
    callback(resp);
}

} // namespace api::controllers
