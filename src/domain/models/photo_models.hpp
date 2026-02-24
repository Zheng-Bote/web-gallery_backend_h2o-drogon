/**
 * SPDX-FileComment: Domain models for Photos and Locations
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file photo_models.hpp
 */

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <optional>
#include <map>

namespace domain::models {

struct Location {
    std::string id;
    std::optional<std::string> continent;
    std::optional<std::string> country;
    std::optional<std::string> province;
    std::optional<std::string> city;
};

struct Photo {
    std::string id;
    std::optional<std::string> location_id;
    std::string file_name;
    std::string file_path;
    std::optional<std::string> thumb_path;
    std::optional<int> width;
    std::optional<int> height;
    std::optional<std::chrono::system_clock::time_point> taken_at;
    std::optional<std::string> camera_make;
    std::optional<std::string> camera_model;
    std::optional<std::string> lens;
    std::optional<int> iso;
    std::optional<float> aperture;
    std::optional<std::string> shutter;
    std::optional<float> focal_length;
    std::optional<double> gps_lat;
    std::optional<double> gps_lon;
    std::chrono::system_clock::time_point created_at;
    
    std::vector<std::string> tags;
    std::map<std::string, std::string> exif;
    std::map<std::string, std::string> iptc;
    std::map<std::string, std::string> xmp;
};

} // namespace domain::models
