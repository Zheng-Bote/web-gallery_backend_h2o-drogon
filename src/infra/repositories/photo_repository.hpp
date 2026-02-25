/**
 * SPDX-FileComment: Photo Repository Header
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file photo_repository.hpp
 * @brief PostgreSQL Implementation of Photo and Location Repositories
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "domain/interfaces/i_photo_repository.hpp"
#include <drogon/orm/DbClient.h>

/**
 * @namespace infra::repositories
 * @brief Namespace for infrastructure data access repositories.
 */
namespace infra::repositories {

using namespace domain::models;
using namespace domain::interfaces;

/**
 * @class PostgresPhotoRepository
 * @brief PostgreSQL implementation of IPhotoRepository.
 */
class PostgresPhotoRepository : public IPhotoRepository {
public:
  std::expected<std::vector<Photo>, std::string>
  find_all(const PhotoFilter &filter) override;
  std::expected<std::optional<Photo>, std::string>
  find_by_id(std::string_view id) override;
  std::expected<void, std::string> save(const Photo &photo) override;
  std::expected<void, std::string> add_tag(std::string_view photo_id,
                                           std::string_view tag) override;
  std::expected<void, std::string> save_metadata_exif(std::string_view photo_id, const std::map<std::string, std::string>& metadata) override;
  std::expected<void, std::string> save_metadata_iptc(std::string_view photo_id, const std::map<std::string, std::string>& metadata) override;
  std::expected<void, std::string> save_metadata_xmp(std::string_view photo_id, const std::map<std::string, std::string>& metadata) override;
};

/**
 * @class PostgresLocationRepository
 * @brief PostgreSQL implementation of ILocationRepository.
 */
class PostgresLocationRepository : public ILocationRepository {
public:
  std::expected<std::vector<Location>, std::string>
  get_tree(bool only_public = false) override;
  std::expected<std::optional<Location>, std::string>
  find_or_create(const Location &loc) override;
};

} // namespace infra::repositories
