/**
 * SPDX-FileComment: TOTP Service Implementation (RFC 6238)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file totp_service.cpp
 * @brief TOTP Service Implementation
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#include "totp_service.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <format>
#include <openssl/evp.h>
#include <openssl/hmac.h>

namespace domain::services {

// Simplified Base32 decoding for TOTP secrets
static std::vector<uint8_t> base32_decode(std::string_view input) {
  static const std::string_view alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  std::vector<uint8_t> out;
  int buffer = 0;
  int bits = 0;

  for (char c : input) {
    auto val = alphabet.find(static_cast<char>(std::toupper(c)));
    if (val == std::string_view::npos)
      continue;
    buffer = (buffer << 5) | (int)val;
    bits += 5;
    if (bits >= 8) {
      bits -= 8;
      out.push_back(static_cast<uint8_t>((buffer >> bits) & 0xFF));
    }
  }
  return out;
}

std::string TotpService::generate_secret() {
  static const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
  std::string secret;
  std::random_device rd;
  for (int i = 0; i < 32; ++i)
    secret += chars[rd() % 32];
  return secret;
}

bool TotpService::validate_code(std::string_view secret,
                                std::string_view code) {
  if (code.size() != 6)
    return false;

  auto key = base32_decode(secret);
  uint64_t counter =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) /
      30;

  auto check = [&](uint64_t c) {
    std::array<uint8_t, 8> msg;
    for (int i = 7; i >= 0; --i) {
      msg[i] = static_cast<uint8_t>(c & 0xFF);
      c >>= 8;
    }

    unsigned int len = 0;
    uint8_t hash[EVP_MAX_MD_SIZE];
    HMAC(EVP_sha1(), key.data(), (int)key.size(), msg.data(), msg.size(), hash,
         &len);

    int offset = hash[len - 1] & 0x0F;
    uint32_t truncated =
        (hash[offset] & 0x7F) << 24 | (hash[offset + 1] & 0xFF) << 16 |
        (hash[offset + 2] & 0xFF) << 8 | (hash[offset + 3] & 0xFF);

    uint32_t final_code = truncated % 1000000;
    return std::format("{:06d}", final_code) == code;
  };

  // Allow window of +/- 1
  return check(counter) || check(counter - 1) || check(counter + 1);
}

std::string TotpService::get_provisioning_uri(std::string_view user,
                                              std::string_view secret,
                                              std::string_view issuer) {
  return std::format("otpauth://totp/{}?secret={}&issuer={}", user, secret,
                     issuer);
}

} // namespace domain::services
