/**
 * SPDX-FileComment: Production-Grade Import Pipeline CLI Tool (Location-Aware
 * Edition) SPDX-FileType: SOURCE SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file import_main.cpp
 * @brief Import CLI tool for processing and indexing photos
 * @version 0.1.4
 * @date 2026-02-25
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "core/config/config_loader.hpp"
#include "domain/models/photo_models.hpp"
#include "infra/util/path_parser.hpp"
#include "infra/repositories/photo_repository.hpp"
#include <Magick++.h>
#include <chrono>
#include <drogon/drogon.h>
#include <exiv2/exiv2.hpp>
#include <filesystem>
#include <iostream>
#include <print>
#include <thread>
#include <uuid/uuid.h>
#include <iomanip>
#include <sstream>
#include <map>
#include <regex>

namespace fs = std::filesystem;
using namespace domain::models;
using namespace core::config;
using namespace infra::repositories;

/**
 * @brief Generates a universally unique identifier (UUID).
 */
std::string generate_uuid() {
  uuid_t b_uuid;
  uuid_generate(b_uuid);
  char out[37];
  uuid_unparse_lower(b_uuid, out);
  return std::string(out);
}

/**
 * @brief Helper to calculate GPS Coordinates from Exiv2 Rational
 */
double get_gps_coordinate(const Exiv2::ExifData& exifData, const char* key, const char* refKey) {
    try {
        auto pos = exifData.findKey(Exiv2::ExifKey(key));
        if (pos == exifData.end() || pos->count() < 3) return 0.0;

        double degrees = pos->toRational(0).first / (double)pos->toRational(0).second;
        double minutes = pos->toRational(1).first / (double)pos->toRational(1).second;
        double seconds = pos->toRational(2).first / (double)pos->toRational(2).second;

        double decimal = degrees + (minutes / 60.0) + (seconds / 3600.0);

        auto refPos = exifData.findKey(Exiv2::ExifKey(refKey));
        if (refPos != exifData.end()) {
            std::string ref = refPos->toString();
            if (ref == "S" || ref == "W") decimal *= -1.0;
        }
        return decimal;
    } catch (...) {
        return 0.0;
    }
}

/**
 * @brief Helper to calculate Altitude
 */
double get_gps_altitude(const Exiv2::ExifData& exifData) {
    try {
        auto pos = exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitude"));
        if (pos == exifData.end() || pos->count() < 1) return 0.0;

        double alt = pos->toRational(0).first / (double)pos->toRational(0).second;

        auto refPos = exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitudeRef"));
        if (refPos != exifData.end()) {
            if (refPos->toLong() == 1) alt *= -1.0;
        }
        return alt;
    } catch (...) {
        return 0.0;
    }
}

/**
 * @brief Parses Exif DateTime string to system_clock::time_point
 */
std::optional<std::chrono::system_clock::time_point> parse_exif_date(const std::string& date_str) {
    if (date_str.empty()) return std::nullopt;
    std::tm tm = {};
    std::istringstream ss(date_str);
    // Exif format is typically "YYYY:MM:DD HH:MM:SS"
    ss >> std::get_time(&tm, "%Y:%m:%d %H:%M:%S");
    if (ss.fail()) return std::nullopt;
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

/**
 * @brief Extracts date from filename using regex patterns
 */
std::optional<std::chrono::system_clock::time_point> parse_date_from_filename(const std::string& filename) {
    // Patterns: YYYYMMDD_HHMMSS or YYYY-MM-DD HH.MM.SS etc.
    static const std::vector<std::pair<std::regex, std::string>> patterns = {
        {std::regex("(\\d{4})(\\d{2})(\\d{2})_(\\d{2})(\\d{2})(\\d{2})"), "%Y%m%d_%H%M%S"},
        {std::regex("(\\d{4})-(\\d{2})-(\\d{2})[ _](\\d{2})[\\.-](\\d{2})[\\.-](\\d{2})"), "%Y-%m-%d %H:%M:%S"}
    };

    for (const auto& [re, fmt] : patterns) {
        std::smatch match;
        if (std::regex_search(filename, match, re)) {
            std::tm tm = {};
            tm.tm_year = std::stoi(match[1]) - 1900;
            tm.tm_mon = std::stoi(match[2]) - 1;
            tm.tm_mday = std::stoi(match[3]);
            tm.tm_hour = std::stoi(match[4]);
            tm.tm_min = std::stoi(match[5]);
            tm.tm_sec = std::stoi(match[6]);
            return std::chrono::system_clock::from_time_t(std::mktime(&tm));
        }
    }
    return std::nullopt;
}

/**
 * @brief Retrieves an existing location ID or creates a new one
 */
std::string get_or_create_location(const infra::util::GeoInfo &geo) {
  auto db = drogon::app().getDbClient("default");

  auto res =
      db->execSqlSync("SELECT id FROM locations WHERE continent = $1 AND "
                      "country = $2 AND province = $3 AND city = $4",
                      geo.continent.value_or(""), geo.country.value_or(""),
                      geo.province.value_or(""), geo.city.value_or(""));

  if (!res.empty()) {
    return res[0]["id"].as<std::string>();
  }

  std::string loc_id = generate_uuid();
  db->execSqlSync("INSERT INTO locations (id, continent, country, province, "
                  "city) VALUES ($1, $2, $3, $4, $5)",
                  loc_id, geo.continent.value_or(""), geo.country.value_or(""),
                  geo.province.value_or(""), geo.city.value_or(""));
  return loc_id;
}

/**
 * @brief Processes a single photo file with robust date fallback logic
 */
void process_photo(const fs::path &base_path, const fs::path &file_path) {
  auto geo_path = infra::util::PathParser::parse(base_path, file_path);
  std::string loc_id = get_or_create_location(geo_path);

  Photo photo;
  photo.id = generate_uuid();
  photo.file_name = file_path.filename().string();
  photo.file_path = file_path.string();
  photo.location_id = loc_id;

  std::map<std::string, std::string> exif_map;
  std::map<std::string, std::string> iptc_map;
  std::map<std::string, std::string> xmp_map;
  std::vector<std::string> tags;

  std::println("--------------------------------------------------");
  std::println("Processing: {}", file_path.filename().string());

  try {
    Magick::Image image;
    image.read(file_path.string());
    photo.width = (int)image.columns();
    photo.height = (int)image.rows();

    try {
      auto exiv_image = Exiv2::ImageFactory::open(file_path.string());
      exiv_image->readMetadata();
      
      // 1. EXIF
      auto &exifData = exiv_image->exifData();
      if (!exifData.empty()) {
          for (auto it = exifData.begin(); it != exifData.end(); ++it) {
              exif_map[it->key()] = it->toString();
          }
          photo.camera_make = exifData["Exif.Image.Make"].toString();
          photo.camera_model = exifData["Exif.Image.Model"].toString();
          
          // Date Fallback Logic Step 1 & 2: EXIF
          photo.taken_at = parse_exif_date(exifData["Exif.Photo.DateTimeOriginal"].toString());
          if (!photo.taken_at) {
              photo.taken_at = parse_exif_date(exifData["Exif.Image.DateTime"].toString());
          }
          
          photo.gps_lat = get_gps_coordinate(exifData, "Exif.GPSInfo.GPSLatitude", "Exif.GPSInfo.GPSLatitudeRef");
          photo.gps_lon = get_gps_coordinate(exifData, "Exif.GPSInfo.GPSLongitude", "Exif.GPSInfo.GPSLongitudeRef");
          photo.gps_alt = get_gps_altitude(exifData);
      }

      // 2. IPTC
      auto &iptcData = exiv_image->iptcData();
      if (!iptcData.empty()) {
          for (auto it = iptcData.begin(); it != iptcData.end(); ++it) {
              iptc_map[it->key()] = it->toString();
              if (it->key() == "Iptc.Application2.Keywords") {
                  tags.push_back(it->toString());
              }
          }
      }

      // 3. XMP
      auto &xmpData = exiv_image->xmpData();
      if (!xmpData.empty()) {
          for (auto it = xmpData.begin(); it != xmpData.end(); ++it) {
              xmp_map[it->key()] = it->toString();
              if (it->key() == "Xmp.dc.subject") {
                  tags.push_back(it->toString());
              }
          }
      }

    } catch (...) {}

    // Date Fallback Logic Step 3: Filename
    if (!photo.taken_at) {
        photo.taken_at = parse_date_from_filename(photo.file_name);
    }

    // Date Fallback Logic Step 4: File System Last Modified
    if (!photo.taken_at) {
        auto ftime = fs::last_write_time(file_path);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        photo.taken_at = sctp;
    }

    std::vector<int> sizes = {480, 680, 800, 1024, 1280};
    fs::path thumb_base = "/data/thumbs";
    for (int size : sizes) {
      fs::path relative_file = fs::relative(file_path, base_path);
      fs::path size_path = thumb_base / std::to_string(size) / relative_file;
      size_path.replace_extension(".webp");
      fs::create_directories(size_path.parent_path());

      Magick::Image thumb = image;
      thumb.resize(Magick::Geometry(size, size));
      thumb.magick("WEBP");
      thumb.write(size_path.string());
    }

    photo.thumb_path =
        fs::relative(file_path, base_path).replace_extension(".webp").string();

    PostgresPhotoRepository repo;
    repo.save(photo);

    if (!exif_map.empty()) repo.save_metadata_exif(photo.id, exif_map);
    if (!iptc_map.empty()) repo.save_metadata_iptc(photo.id, iptc_map);
    if (!xmp_map.empty())  repo.save_metadata_xmp(photo.id, xmp_map);

    for (const auto& tag : tags) {
        repo.add_tag(photo.id, tag);
    }

    std::println("  ✓ Successfully imported with full metadata and resolved date.");

  } catch (const std::exception &e) {
    std::println(stderr, "  ✗ Error processing {}: {}",
                 file_path.filename().string(), e.what());
  }
}

/**
 * @brief Main entry point
 */
int main(int argc, char **argv) {
  if (argc < 2) {
    std::println("Usage: gallery-import <directory>");
    return 1;
  }

  ConfigLoader::load("/app/.env");
  Magick::InitializeMagick(*argv);
  std::string root_path = argv[1];

  Json::Value config;
  Json::Value db_client;
  db_client["name"] = "default";
  db_client["rdbms"] = "postgresql";
  db_client["host"] = ConfigLoader::get("DB_HOST", "psql_db");
  db_client["port"] = std::stoi(ConfigLoader::get("DB_PORT", "5432"));
  db_client["dbname"] = ConfigLoader::get("DB_NAME", "gallery");
  db_client["user"] = ConfigLoader::get("DB_USER", "gallery_user");
  db_client["passwd"] = ConfigLoader::get("DB_PASSWORD", "");
  db_client["connection_number"] = 1;
  config["db_clients"].append(db_client);
  drogon::app().loadConfigJson(config);

  std::thread worker([root_path]() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto db = drogon::app().getDbClient("default");
    if (!db) {
      std::println(stderr, "Fatal: DB pool not ready.");
      drogon::app().quit();
      return;
    }

    try {
      db->execSqlSync("SELECT 1");
      std::println("Database connected. Starting scan...");
    } catch (const std::exception &e) {
      std::println(stderr, "Fatal: Connection failed: {}", e.what());
      drogon::app().quit();
      return;
    }

    fs::path root = root_path;
    for (const auto &entry : fs::recursive_directory_iterator(root)) {
      if (entry.is_regular_file()) {
        std::string ext = entry.path().extension().string();
        for (auto &c : ext)
          c = (char)std::tolower(c);
        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png") {
          process_photo(root, entry.path());
        }
      }
    }
    std::println("--------------------------------------------------");
    std::println("Import complete.");
    drogon::app().quit();
  });

  drogon::app().run();
  worker.join();
  return 0;
}
