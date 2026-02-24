/**
 * SPDX-FileComment: Password Service (Argon2id)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file password_service.hpp
 */

#pragma once

#include <string>
#include <string_view>
#include <expected>

namespace domain::services {

/**
 * @class PasswordService
 * @brief Handles secure password hashing using Argon2id.
 */
class PasswordService {
public:
    /**
     * @brief Hashes a plain text password.
     */
    static std::expected<std::string, std::string> hash_password(std::string_view password);

    /**
     * @brief Verifies a password against a hash.
     */
    static bool verify_password(std::string_view password, std::string_view hash);
};

} // namespace domain::services
