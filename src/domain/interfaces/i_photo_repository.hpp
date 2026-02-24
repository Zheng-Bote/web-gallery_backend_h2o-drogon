/**
 * SPDX-FileComment: Photo Repository Interface
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file i_photo_repository.hpp
 * @brief Interfaces for Photo and Location Repositories
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "domain/models/photo_models.hpp"
#include <expected>
#include <optional>
#include <string>
#include <vector>

/**
 * @namespace domain::interfaces
 * @brief Namespace for domain interfaces.
 */
namespace domain::interfaces {

using namespace domain::models;

/**
 * @struct PhotoFilter
 * @brief Filter criteria for querying photos.
 */
struct PhotoFilter {
  std::optional<std::string> location_id;
  std::optional<std::string> tag;
  int offset = 0;
  int limit = 20;
};

/**
 * @class IPhotoRepository
 * @brief Interface for photo data access operations.
 */
class IPhotoRepository {
public:
  virtual ~IPhotoRepository() = default;

  virtual std::expected<std::vector<Photo>, std::string>
  find_all(const PhotoFilter &filter) = 0;
  virtual std::expected<std::optional<Photo>, std::string>
  find_by_id(std::string_view id) = 0;
  virtual std::expected<void, std::string> save(const Photo &photo) = 0;
  virtual std::expected<void, std::string> add_tag(std::string_view photo_id,
                                                   std::string_view tag) = 0;
};

/**
 * @class ILocationRepository
 * @brief Interface for location data access operations.
 */
class ILocationRepository {
public:
  virtual ~ILocationRepository() = default;

  virtual std::expected<std::vector<Location>, std::string> get_tree() = 0;
  virtual std::expected<std::optional<Location>, std::string>
  find_or_create(const Location &loc) = 0;
};

} // namespace domain::interfaces
