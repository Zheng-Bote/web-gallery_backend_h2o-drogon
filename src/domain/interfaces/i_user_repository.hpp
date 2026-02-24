/**
 * SPDX-FileComment: User Repository Interface
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file i_user_repository.hpp
 */

#pragma once

#include "domain/models/user_models.hpp"
#include <expected>
#include <string>
#include <vector>
#include <memory>

namespace domain::interfaces {

using namespace domain::models;

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual std::expected<std::optional<User>, std::string> find_by_username(std::string_view username) = 0;
    virtual std::expected<std::optional<User>, std::string> find_by_id(std::string_view id) = 0;
    virtual std::expected<void, std::string> save(const User& user) = 0;
    virtual std::expected<std::vector<Permission>, std::string> get_user_permissions(std::string_view user_id) = 0;
    virtual std::expected<std::vector<CommunicationChannel>, std::string> get_user_channels(std::string_view user_id) = 0;
};

} // namespace domain::interfaces
