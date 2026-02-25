/**
 * SPDX-FileComment: Photo Repository Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file photo_repository.cpp
 * @brief PostgreSQL Implementation of Photo Repository
 * @version 0.1.12
 * @date 2026-02-25
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "photo_repository.hpp"
#include <drogon/drogon.h>
#include <json/json.h>

namespace infra::repositories {

static std::vector<Photo> map_photo_result(const drogon::orm::Result &result) {
  std::vector<Photo> photos;
  for (const auto &row : result) {
    Photo p;
    p.id = row["id"].template as<std::string>();
    p.file_name = row["file_name"].template as<std::string>();
    p.file_path = row["file_path"].template as<std::string>();
    p.thumb_path = row["thumb_path"].template as<std::string>();
    p.width = row["width"].template as<int>();
    p.height = row["height"].template as<int>();
    p.camera_make = row["camera_make"].template as<std::string>();
    p.camera_model = row["camera_model"].template as<std::string>();
    if (!row["gps_lat"].isNull()) p.gps_lat = row["gps_lat"].template as<double>();
    if (!row["gps_lon"].isNull()) p.gps_lon = row["gps_lon"].template as<double>();
    if (!row["gps_alt"].isNull()) p.gps_alt = row["gps_alt"].template as<double>();
    p.is_public = row["is_public"].template as<bool>();
    photos.push_back(p);
  }
  return photos;
}

// Helper to convert optional to Json::Value (supporting null)
template<typename T>
static Json::Value to_json_param(const std::optional<T>& opt) {
    if (opt) return Json::Value(*opt);
    return Json::Value(Json::nullValue);
}

std::expected<std::vector<Photo>, std::string>
PostgresPhotoRepository::find_all(const PhotoFilter &filter) {
  auto db = drogon::app().getDbClient();
  try {
    std::string sql = "SELECT id, file_name, file_path, thumb_path, width, "
                      "height, camera_make, camera_model, gps_lat, gps_lon, gps_alt, is_public FROM photos";

    // Use strings for pagination to avoid protocol mismatches
    std::string limit_str = std::to_string(filter.limit);
    std::string offset_str = std::to_string(filter.offset);

    if (filter.location_id && filter.is_public) {
      std::string loc_id = *filter.location_id;
      std::string is_pub = (*filter.is_public ? "true" : "false");
      return map_photo_result(db->execSqlSync(sql + " WHERE location_id = $1::uuid AND is_public = $2::boolean LIMIT $3::int OFFSET $4::int",
                               loc_id, is_pub, limit_str, offset_str));
    } else if (filter.location_id) {
      std::string loc_id = *filter.location_id;
      return map_photo_result(db->execSqlSync(sql + " WHERE location_id = $1::uuid LIMIT $2::int OFFSET $3::int",
                               loc_id, limit_str, offset_str));
    } else if (filter.is_public) {
      std::string is_pub = (*filter.is_public ? "true" : "false");
      return map_photo_result(db->execSqlSync(sql + " WHERE is_public = $1::boolean LIMIT $2::int OFFSET $3::int",
                               is_pub, limit_str, offset_str));
    } else {
      return map_photo_result(db->execSqlSync(sql + " LIMIT $1::int OFFSET $2::int",
                               limit_str, offset_str));
    }
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<std::optional<Photo>, std::string>
PostgresPhotoRepository::find_by_id(std::string_view id) {
  if (id.empty()) return std::nullopt;

  auto db = drogon::app().getDbClient();
  try {
    auto result =
        db->execSqlSync("SELECT * FROM photos WHERE id = $1::uuid", std::string(id));
    if (result.empty())
      return std::nullopt;

    const auto &row = result[0];
    Photo p;
    p.id = row["id"].template as<std::string>();
    p.file_name = row["file_name"].template as<std::string>();
    p.file_path = row["file_path"].template as<std::string>();
    p.thumb_path = row["thumb_path"].template as<std::string>();
    p.width = row["width"].template as<int>();
    p.height = row["height"].template as<int>();
    p.camera_make = row["camera_make"].template as<std::string>();
    p.camera_model = row["camera_model"].template as<std::string>();
    if (!row["gps_lat"].isNull()) p.gps_lat = row["gps_lat"].template as<double>();
    if (!row["gps_lon"].isNull()) p.gps_lon = row["gps_lon"].template as<double>();
    if (!row["gps_alt"].isNull()) p.gps_alt = row["gps_alt"].template as<double>();
    p.is_public = row["is_public"].template as<bool>();
    return p;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<void, std::string>
PostgresPhotoRepository::save(const Photo &photo) {
  auto db = drogon::app().getDbClient();
  try {
    db->execSqlSync("INSERT INTO photos (id, location_id, file_name, file_path, thumb_path, "
                    "width, height, camera_make, camera_model, gps_lat, gps_lon, gps_alt, is_public) "
                    "VALUES ($1::uuid, $2::uuid, $3, $4, $5, $6::int, $7::int, $8, $9, $10::double precision, $11::double precision, $12::double precision, $13::boolean) "
                    "ON CONFLICT (file_path) DO UPDATE SET thumb_path = "
                    "EXCLUDED.thumb_path, is_public = EXCLUDED.is_public, "
                    "gps_lat = EXCLUDED.gps_lat, gps_lon = EXCLUDED.gps_lon, gps_alt = EXCLUDED.gps_alt",
                    photo.id, 
                    to_json_param(photo.location_id), 
                    photo.file_name, 
                    photo.file_path,
                    to_json_param(photo.thumb_path), 
                    photo.width.value_or(0),
                    photo.height.value_or(0), 
                    to_json_param(photo.camera_make),
                    to_json_param(photo.camera_model), 
                    to_json_param(photo.gps_lat), 
                    to_json_param(photo.gps_lon), 
                    to_json_param(photo.gps_alt), 
                    photo.is_public);
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
    db->execSqlSync("INSERT INTO photo_tags (photo_id, tag) VALUES ($1::uuid, $2) ON "
                    "CONFLICT DO NOTHING",
                    std::string(photo_id), std::string(tag));
    return {};
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<void, std::string>
PostgresPhotoRepository::save_metadata_exif(std::string_view photo_id, const std::map<std::string, std::string>& metadata) {
  auto db = drogon::app().getDbClient();
  try {
    for (const auto& [key, value] : metadata) {
      db->execSqlSync("INSERT INTO photo_metadata_exif (photo_id, key, value) VALUES ($1::uuid, $2, $3) "
                      "ON CONFLICT (photo_id, key) DO UPDATE SET value = EXCLUDED.value",
                      std::string(photo_id), key, value);
    }
    return {};
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<void, std::string>
PostgresPhotoRepository::save_metadata_iptc(std::string_view photo_id, const std::map<std::string, std::string>& metadata) {
  auto db = drogon::app().getDbClient();
  try {
    for (const auto& [key, value] : metadata) {
      db->execSqlSync("INSERT INTO photo_metadata_iptc (photo_id, key, value) VALUES ($1::uuid, $2, $3) "
                      "ON CONFLICT (photo_id, key) DO UPDATE SET value = EXCLUDED.value",
                      std::string(photo_id), key, value);
    }
    return {};
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

std::expected<void, std::string>
PostgresPhotoRepository::save_metadata_xmp(std::string_view photo_id, const std::map<std::string, std::string>& metadata) {
  auto db = drogon::app().getDbClient();
  try {
    for (const auto& [key, value] : metadata) {
      db->execSqlSync("INSERT INTO photo_metadata_xmp (photo_id, key, value) VALUES ($1::uuid, $2, $3) "
                      "ON CONFLICT (photo_id, key) DO UPDATE SET value = EXCLUDED.value",
                      std::string(photo_id), key, value);
    }
    return {};
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

// Location Repository
std::expected<std::vector<Location>, std::string>
PostgresLocationRepository::get_tree(bool only_public) {
  auto db = drogon::app().getDbClient();
  try {
    std::string sql;
    if (only_public) {
      sql = "SELECT DISTINCT l.continent, l.country, l.province, l.city "
            "FROM locations l "
            "JOIN photos p ON l.id = p.location_id "
            "WHERE p.is_public = TRUE "
            "ORDER BY l.continent, l.country, l.province, l.city";
    } else {
      sql = "SELECT continent, country, province, city FROM locations ORDER BY "
            "continent, country, province, city";
    }
    auto result = db->execSqlSync(sql);
    std::vector<Location> locs;
    for (const auto &row : result) {
      Location l;
      l.continent = row["continent"].template as<std::string>();
      l.country = row["country"].template as<std::string>();
      l.province = row["province"].template as<std::string>();
      l.city = row["city"].template as<std::string>();
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
    res.id = result[0]["id"].template as<std::string>();
    return res;
  } catch (const std::exception &e) {
    return std::unexpected(e.what());
  }
}

} // namespace infra::repositories
