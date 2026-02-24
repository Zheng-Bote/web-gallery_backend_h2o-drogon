/**
 * SPDX-FileComment: Photo Repository Interface
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file i_photo_repository.hpp
 */

#pragma once

#include "domain/models/photo_models.hpp"
#include <expected>
#include <string>
#include <vector>
#include <optional>

namespace domain::interfaces {

using namespace domain::models;

struct PhotoFilter {
    std::optional<std::string> location_id;
    std::optional<std::string> tag;
    int offset = 0;
    int limit = 20;
};

class IPhotoRepository {
public:
    virtual ~IPhotoRepository() = default;

    virtual std::expected<std::vector<Photo>, std::string> find_all(const PhotoFilter& filter) = 0;
    virtual std::expected<std::optional<Photo>, std::string> find_by_id(std::string_view id) = 0;
    virtual std::expected<void, std::string> save(const Photo& photo) = 0;
    virtual std::expected<void, std::string> add_tag(std::string_view photo_id, std::string_view tag) = 0;
};

class ILocationRepository {
public:
    virtual ~ILocationRepository() = default;

    virtual std::expected<std::vector<Location>, std::string> get_tree() = 0;
    virtual std::expected<std::optional<Location>, std::string> find_or_create(const Location& loc) = 0;
};

} // namespace domain::interfaces
