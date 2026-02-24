/**
 * SPDX-FileComment: Password Service Implementation (Argon2id)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file password_service.cpp
 * @brief Password Service Implementation Using Argon2id
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "password_service.hpp"
#include <argon2.h>
#include <format>
#include <random>
#include <vector>

namespace domain::services {

static constexpr size_t SALT_LEN = 16;
static constexpr uint32_t T_COST = 2;
static constexpr uint32_t M_COST = 65536;
static constexpr uint32_t PARALLELISM = 1;

std::expected<std::string, std::string>
PasswordService::hash_password(std::string_view password) {
  std::vector<uint8_t> salt(SALT_LEN);
  std::random_device rd;
  std::generate(salt.begin(), salt.end(), std::ref(rd));

  char encoded[256];
  int res = argon2id_hash_encoded(T_COST, M_COST, PARALLELISM, password.data(),
                                  password.size(), salt.data(), salt.size(), 32,
                                  encoded, sizeof(encoded));

  if (res != ARGON2_OK) {
    return std::unexpected(
        std::format("Argon2 hashing failed: {}", argon2_error_message(res)));
  }

  return std::string(encoded);
}

bool PasswordService::verify_password(std::string_view password,
                                      std::string_view hash) {
  int res = argon2id_verify(hash.data(), password.data(), password.size());
  return res == ARGON2_OK;
}

} // namespace domain::services
