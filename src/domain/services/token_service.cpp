/**
 * SPDX-FileComment: Token Service Implementation (JWT)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file token_service.cpp
 * @brief Token Service Implementation (JWT)
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "token_service.hpp"
#include <chrono>
#include <jwt-cpp/jwt.h>

namespace domain::services {

std::expected<std::string, std::string>
TokenService::create_token(const TokenPayload &payload,
                           std::string_view secret) {
  try {
    auto token =
        jwt::create()
            .set_issuer("PhotoGallery")
            .set_type("JWS")
            .set_subject(payload.user_id)
            .set_payload_claim("username", jwt::claim(payload.username))
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() +
                            std::chrono::hours{24})
            .sign(jwt::algorithm::hs256{std::string(secret)});

    return token;
  } catch (const std::exception &e) {
    return std::unexpected(std::format("JWT creation failed: {}", e.what()));
  }
}

std::expected<TokenPayload, std::string>
TokenService::verify_token(std::string_view token, std::string_view secret) {
  try {
    auto decoded = jwt::decode(std::string(token));

    auto verifier =
        jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{std::string(secret)})
            .with_issuer("PhotoGallery");

    verifier.verify(decoded);

    TokenPayload payload;
    payload.user_id = decoded.get_subject();
    payload.username = decoded.get_payload_claim("username").as_string();

    // Roles extraction if present
    // payload.roles = ...

    return payload;
  } catch (const std::exception &e) {
    return std::unexpected(
        std::format("JWT verification failed: {}", e.what()));
  }
}

} // namespace domain::services
