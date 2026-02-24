/**
 * SPDX-FileComment: User Repository Header
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file user_repository.hpp
 * @brief PostgreSQL Implementation of User Repository
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include "domain/interfaces/i_user_repository.hpp"
#include <drogon/orm/DbClient.h>

/**
 * @namespace infra::repositories
 * @brief Namespace for infrastructure data access repositories.
 */
namespace infra::repositories {

using namespace domain::models;
using namespace domain::interfaces;

/**
 * @class PostgresUserRepository
 * @brief PostgreSQL implementation of IUserRepository.
 */
class PostgresUserRepository : public IUserRepository {
public:
  std::expected<std::optional<User>, std::string>
  find_by_username(std::string_view username) override;
  std::expected<std::optional<User>, std::string>
  find_by_id(std::string_view id) override;
  std::expected<void, std::string> save(const User &user) override;
  std::expected<std::vector<Permission>, std::string>
  get_user_permissions(std::string_view user_id) override;
  std::expected<std::vector<CommunicationChannel>, std::string>
  get_user_channels(std::string_view user_id) override;
};

} // namespace infra::repositories
