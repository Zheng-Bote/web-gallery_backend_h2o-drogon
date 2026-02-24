/**
 * SPDX-FileComment: Production-Grade Import Pipeline CLI Tool (Location-Aware
 * Edition) SPDX-FileType: SOURCE SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file import_main.cpp
 * @brief Import CLI tool for processing and indexing photos
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "core/config/config_loader.hpp"
#include "domain/models/photo_models.hpp"
#include "infra/util/path_parser.hpp"
#include <Magick++.h>
#include <chrono>
#include <drogon/drogon.h>
#include <exiv2/exiv2.hpp>
#include <filesystem>
#include <iostream>
#include <print>
#include <thread>
#include <uuid/uuid.h>

namespace fs = std::filesystem;
using namespace domain::models;
using namespace core::config;

/**
 * @brief Generates a universally unique identifier (UUID).
 *
 * @return std::string The generated UUID.
 */
std::string generate_uuid() {
  uuid_t b_uuid;
  uuid_generate(b_uuid);
  char out[37];
  uuid_unparse_lower(b_uuid, out);
  return std::string(out);
}

/**
 * @brief Retrieves an existing location ID or creates a new one based on geo
 * information.
 *
 * @param geo The geographic information.
 * @return std::string The location ID.
 */
std::string get_or_create_location(const infra::util::GeoInfo &geo) {
  auto db = drogon::app().getDbClient("default");

  // Check if exists
  auto res =
      db->execSqlSync("SELECT id FROM locations WHERE continent = $1 AND "
                      "country = $2 AND province = $3 AND city = $4",
                      geo.continent.value_or(""), geo.country.value_or(""),
                      geo.province.value_or(""), geo.city.value_or(""));

  if (!res.empty()) {
    return res[0]["id"].as<std::string>();
  }

  // Create new
  std::string loc_id = generate_uuid();
  db->execSqlSync("INSERT INTO locations (id, continent, country, province, "
                  "city) VALUES ($1, $2, $3, $4, $5)",
                  loc_id, geo.continent.value_or(""), geo.country.value_or(""),
                  geo.province.value_or(""), geo.city.value_or(""));
  return loc_id;
}

/**
 * @brief Processes a single photo file, extracting metadata and generating
 * thumbnails.
 *
 * @param base_path The base directory path.
 * @param file_path The path to the photo file.
 */
void process_photo(const fs::path &base_path, const fs::path &file_path) {
  auto geo = infra::util::PathParser::parse(base_path, file_path);
  std::string loc_id = get_or_create_location(geo);

  Photo photo;
  photo.id = generate_uuid();
  photo.file_name = file_path.filename().string();
  photo.file_path = file_path.string();
  photo.location_id = loc_id;

  std::println("--------------------------------------------------");
  std::println("Processing: {}", file_path.filename().string());

  try {
    Magick::Image image;
    image.read(file_path.string());
    photo.width = (int)image.columns();
    photo.height = (int)image.rows();

    try {
      auto exif = Exiv2::ImageFactory::open(file_path.string());
      exif->readMetadata();
      auto &exifData = exif->exifData();
      photo.camera_make = exifData["Exif.Image.Make"].toString();
      photo.camera_model = exifData["Exif.Image.Model"].toString();
    } catch (...) {
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

    auto db = drogon::app().getDbClient("default");
    db->execSqlSync(
        "INSERT INTO photos (id, location_id, file_name, file_path, "
        "thumb_path, width, height, camera_make, camera_model) "
        "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
        photo.id, photo.location_id.value(), photo.file_name, photo.file_path,
        photo.thumb_path, photo.width.value_or(0), photo.height.value_or(0),
        photo.camera_make.value_or(""), photo.camera_model.value_or(""));

    std::println("  ✓ Successfully imported with location indexing.");

  } catch (const std::exception &e) {
    std::println(stderr, "  ✗ Error processing {}: {}",
                 file_path.filename().string(), e.what());
  }
}

/**
 * @brief Main entry point for the gallery import CLI tool.
 *
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return int The exit status.
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
