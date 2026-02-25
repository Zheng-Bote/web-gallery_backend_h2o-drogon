/**
 * SPDX-FileComment: Location Controller Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file location_controller.cpp
 * @brief Location Controller Implementation file
 * @version 0.1.3
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "location_controller.hpp"
#include "infra/repositories/photo_repository.hpp"
#include <drogon/HttpResponse.h>
#include <map>
#include <nlohmann/json.hpp>

namespace api::controllers {

void LocationController::get_tree(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
  infra::repositories::PostgresLocationRepository repo;

  // Attributes::get is safe here as OptionalAuthMiddleware always sets it
  bool is_authenticated = req->attributes()->get<bool>("is_authenticated");
  auto res = repo.get_tree(!is_authenticated);

  if (!res) {
    nlohmann::json error_json = {{"error", res.error()}};
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setBody(error_json.dump());
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
    callback(resp);
    return;
  }

  nlohmann::json root;
  auto &continents_arr = root["continents"] = nlohmann::json::array();

  // Map-Struktur zum Gruppieren: Continent -> Country -> Province -> Set of
  // Cities
  std::map<
      std::string,
      std::map<std::string, std::map<std::string, std::vector<std::string>>>>
      tree;

  for (const auto &l : res.value()) {
    std::string cont = l.continent.value_or("Unknown");
    std::string country = l.country.value_or("Unknown");
    std::string prov = l.province.value_or("Unknown");
    std::string city = l.city.value_or("Unknown");

    tree[cont][country][prov].push_back(city);
  }

  // JSON-Struktur aufbauen
  for (auto const &[cont_name, countries] : tree) {
    nlohmann::json cont_json = {{"name", cont_name},
                                {"countries", nlohmann::json::array()}};

    for (auto const &[country_name, provinces] : countries) {
      nlohmann::json country_json = {{"name", country_name},
                                     {"provinces", nlohmann::json::array()}};

      for (auto const &[prov_name, cities] : provinces) {
        country_json["provinces"].push_back(
            {{"name", prov_name}, {"cities", cities}});
      }
      cont_json["countries"].push_back(country_json);
    }
    continents_arr.push_back(cont_json);
  }

  auto resp = drogon::HttpResponse::newHttpResponse();
  resp->setBody(root.dump());
  resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
  callback(resp);
}

} // namespace api::controllers
