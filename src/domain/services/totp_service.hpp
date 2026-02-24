/**
 * SPDX-FileComment: TOTP Service (RFC 6238)
 * SPDX-FileType: HEADER
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file totp_service.hpp
 * @brief TOTP Service (RFC 6238)
 * @version 0.1.0
 * @date 2026-02-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license Apache-2.0
 */

#pragma once

#include <expected>
#include <string>
#include <string_view>

namespace domain::services {

class TotpService {
public:
  /**
   * @brief Generates a new TOTP secret.
   */
  static std::string generate_secret();

  /**
   * @brief Validates a TOTP code against a secret.
   */
  static bool validate_code(std::string_view secret, std::string_view code);

  /**
   * @brief Generates a provisioning URI for QR codes.
   */
  static std::string get_provisioning_uri(std::string_view user,
                                          std::string_view secret,
                                          std::string_view issuer);
};

} // namespace domain::services
