/**
 * ESPTeamsPresence -- A standalone Microsoft Teams presence light
 *   based on ESP32 and RGB neopixel LEDs.
 *   https://github.com/toblum/ESPTeamsPresence
 *
 * Copyright (C) 2020 Tobias Blum <make@tobiasblum.de>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * refactored by @riraosan.github.io
 * If I have seen further it is by standing on the shoulders of giants.
 */

#pragma once

#include "driver/rmt.h"
#include <Arduino.h>

class ESP32_RMT_Driver {
public:
  ESP32_RMT_Driver() {}
  ~ESP32_RMT_Driver() {}

  /*
   * Initialize the RMT Tx channel
   */
  void begin(rmt_channel_t channel, uint8_t gpio);
};
