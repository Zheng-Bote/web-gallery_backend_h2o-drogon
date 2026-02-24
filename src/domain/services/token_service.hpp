/**
 * SPDX-FileComment: Token Service (JWT)
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file token_service.hpp
 * @brief Token Service (JWT) Header
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <chrono>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

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
  static std::expected<std::string, std::string>
  create_token(const TokenPayload &payload, std::string_view secret);
  static std::expected<TokenPayload, std::string>
  verify_token(std::string_view token, std::string_view secret);
};

} // namespace domain::services
