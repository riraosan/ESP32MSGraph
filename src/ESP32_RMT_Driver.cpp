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



#include "driver/rmt.h"
#include <Arduino.h>
#include "ESP32_RMT_Driver.h"
#include "esp32-hal-log.h"

#define APB_CLK_MHZ 80                                  // default RMT CLK source (80MHz)
#define RMT_CLK_DIV 2                                   // RMT CLK divider
#define RMT_TICK    (RMT_CLK_DIV * 1000 / APB_CLK_MHZ)  // 25ns
#define T1_TICKS    (250 / RMT_TICK)                    // 250ns
#define T2_TICKS    (625 / RMT_TICK)                    // 625ns
#define T3_TICKS    (375 / RMT_TICK)                    // 375ns
#define RESET_TICKS (50000 / RMT_TICK)                  // 50us

extern "C" void IRAM_ATTR u8_to_rmt(const void* src, rmt_item32_t* dest, size_t src_size, size_t wanted_num, size_t* translated_size, size_t* item_num) {
  if (src == NULL || dest == NULL) {
    *translated_size = 0;
    *item_num        = 0;
    return;
  }

  const rmt_item32_t bit0  = {{{T1_TICKS, 1, T2_TICKS + T3_TICKS, 0}}};     // Logical 0
  const rmt_item32_t bit1  = {{{T1_TICKS + T2_TICKS, 1, T3_TICKS, 0}}};     // Logical 1
  const rmt_item32_t reset = {{{RESET_TICKS / 2, 0, RESET_TICKS / 2, 0}}};  // Reset
  size_t             size  = 0;
  size_t             num   = 0;
  uint8_t*           psrc  = (uint8_t*)src;
  rmt_item32_t*      pdest = dest;

  while (size < src_size && num < wanted_num) {
    if (size < src_size - 1) {  // have more pixel data, so translate into RMT items
      (pdest++)->val = (*psrc & B10000000) ? bit1.val : bit0.val;
      (pdest++)->val = (*psrc & B01000000) ? bit1.val : bit0.val;
      (pdest++)->val = (*psrc & B00100000) ? bit1.val : bit0.val;
      (pdest++)->val = (*psrc & B00010000) ? bit1.val : bit0.val;
      (pdest++)->val = (*psrc & B00001000) ? bit1.val : bit0.val;
      (pdest++)->val = (*psrc & B00000100) ? bit1.val : bit0.val;
      (pdest++)->val = (*psrc & B00000010) ? bit1.val : bit0.val;
      (pdest++)->val = (*psrc & B00000001) ? bit1.val : bit0.val;
      num += 8;
    } else {  // no more pixel data, last RMT item is the reset pulse
      (pdest++)->val = reset.val;
      num++;
    }
    size++;
    psrc++;
  }

  *translated_size = size;
  *item_num        = num;
}

void ESP32_RMT_Driver::begin(rmt_channel_t channel, uint8_t gpio) {
  log_i("%d, %d, %d, %d, %d, %d, %d", APB_CLK_MHZ, RMT_CLK_DIV, RMT_TICK, T1_TICKS, T2_TICKS, T3_TICKS, RESET_TICKS);

  rmt_config_t config;
  config.rmt_mode                 = RMT_MODE_TX;
  config.channel                  = channel;
  config.gpio_num                 = (gpio_num_t)gpio;
  config.clk_div                  = RMT_CLK_DIV;
  config.mem_block_num            = 1;  // 64 pulse "items" per block
  config.tx_config.loop_en        = 0;
  config.tx_config.carrier_en     = 0;
  config.tx_config.idle_output_en = 1;
  config.tx_config.idle_level     = RMT_IDLE_LEVEL_LOW;

  rmt_config(&config);
  rmt_driver_install(config.channel, 0, 0);
  rmt_translator_init(config.channel, u8_to_rmt);
}
