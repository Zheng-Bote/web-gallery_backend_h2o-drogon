/**
 * SPDX-FileComment: Photo Repository Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file photo_repository.cpp
 * @brief PostgreSQL Implementation of Photo Repository
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "photo_repository.hpp"
#include <drogon/drogon.h>

namespace infra::repositories {

std::expected<std::vector<Photo>, std::string>
PostgresPhotoRepository::find_all(const PhotoFilter &filter) {
  auto db = drogon::app().getDbClient();
  try {
    auto result = db->execSqlSync(
        "SELECT id, file_name, file_path, thumb_path, width, height, "
        "camera_make, camera_model FROM photos LIMIT $1 OFFSET $2",
        filter.limit, filter.offset);

    std::vector<Photo> photos;
    for (const auto &row : result) {
      Photo p;
      p.id = row["id"].as<std::string>();
      p.file_name = row["file_name"].as<std::string>();
      p.file_path = row["file_path"].as<std::string>();
      p.thumb_path = row["thumb_path"].as<std::string>();
      p.width = row["width"].as<int>();
      p.height = row["height"].as<int>();
      p.camera_make = row["camera_make"].as<std::string>();
      p.camera_model = row["camera_model"].as<std::string>();
      photos.push_back(p);
    }
    return photos;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<std::optional<Photo>, std::string>
PostgresPhotoRepository::find_by_id(std::string_view id) {
  auto db = drogon::app().getDbClient();
  try {
    auto result =
        db->execSqlSync("SELECT * FROM photos WHERE id = $1", std::string(id));
    if (result.empty())
      return std::nullopt;

    const auto &row = result[0];
    Photo p;
    p.id = row["id"].as<std::string>();
    // ... map other fields
    return p;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<void, std::string>
PostgresPhotoRepository::save(const Photo &photo) {
  // Already mostly implemented in import_main.cpp but encapsulated here
  auto db = drogon::app().getDbClient();
  try {
    db->execSqlSync("INSERT INTO photos (id, file_name, file_path, thumb_path, "
                    "width, height, camera_make, camera_model) "
                    "VALUES ($1, $2, $3, $4, $5, $6, $7, $8) "
                    "ON CONFLICT (file_path) DO UPDATE SET thumb_path = "
                    "EXCLUDED.thumb_path",
                    photo.id, photo.file_name, photo.file_path,
                    photo.thumb_path.value_or(""), photo.width.value_or(0),
                    photo.height.value_or(0), photo.camera_make.value_or(""),
                    photo.camera_model.value_or(""));
    return {};
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<void, std::string>
PostgresPhotoRepository::add_tag(std::string_view photo_id,
                                 std::string_view tag) {
  auto db = drogon::app().getDbClient();
  try {
    db->execSqlSync("INSERT INTO photo_tags (photo_id, tag) VALUES ($1, $2) ON "
                    "CONFLICT DO NOTHING",
                    std::string(photo_id), std::string(tag));
    return {};
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

// Location Repository
std::expected<std::vector<Location>, std::string>
PostgresLocationRepository::get_tree() {
  auto db = drogon::app().getDbClient();
  try {
    auto result = db->execSqlSync(
        "SELECT continent, country, province, city FROM locations ORDER BY "
        "continent, country, province, city");
    std::vector<Location> locs;
    for (const auto &row : result) {
      Location l;
      l.continent = row["continent"].as<std::string>();
      l.country = row["country"].as<std::string>();
      l.province = row["province"].as<std::string>();
      l.city = row["city"].as<std::string>();
      locs.push_back(l);
    }
    return locs;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<std::optional<Location>, std::string>
PostgresLocationRepository::find_or_create(const Location &loc) {
  auto db = drogon::app().getDbClient();
  try {
    auto result = db->execSqlSync(
        "INSERT INTO locations (continent, country, province, city) "
        "VALUES ($1, $2, $3, $4) "
        "ON CONFLICT (continent, country, province, city) DO UPDATE SET "
        "continent = EXCLUDED.continent "
        "RETURNING id",
        loc.continent.value_or(""), loc.country.value_or(""),
        loc.province.value_or(""), loc.city.value_or(""));

    Location res = loc;
    res.id = result[0]["id"].as<std::string>();
    return res;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

} // namespace infra::repositories
