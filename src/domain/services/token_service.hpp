/**
 * SPDX-FileComment: Token Service (JWT)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file token_service.hpp
 */

#pragma once

#include <string>
#include <string_view>
#include <expected>
#include <vector>
#include <chrono>

namespace domain::services {

struct TokenPayload {
    std::string user_id;
    std::string username;
    std::vector<std::string> roles;
};

/**
 * @class TokenService
 * @brief Handles JWT creation and validation.
 */
class TokenService {
public:
    static std::expected<std::string, std::string> create_token(const TokenPayload& payload, std::string_view secret);
    static std::expected<TokenPayload, std::string> verify_token(std::string_view token, std::string_view secret);
};

} // namespace domain::services
