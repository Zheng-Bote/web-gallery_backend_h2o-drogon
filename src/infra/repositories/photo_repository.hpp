/**
 * SPDX-FileComment: Photo Repository Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "domain/interfaces/i_photo_repository.hpp"
#include <drogon/orm/DbClient.h>

namespace infra::repositories {

using namespace domain::models;
using namespace domain::interfaces;

class PostgresPhotoRepository : public IPhotoRepository {
public:
    std::expected<std::vector<Photo>, std::string> find_all(const PhotoFilter& filter) override;
    std::expected<std::optional<Photo>, std::string> find_by_id(std::string_view id) override;
    std::expected<void, std::string> save(const Photo& photo) override;
    std::expected<void, std::string> add_tag(std::string_view photo_id, std::string_view tag) override;
};

class PostgresLocationRepository : public ILocationRepository {
public:
    std::expected<std::vector<Location>, std::string> get_tree() override;
    std::expected<std::optional<Location>, std::string> find_or_create(const Location& loc) override;
};

} // namespace infra::repositories
