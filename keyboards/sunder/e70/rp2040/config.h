// Copyright 2023 squishyliquid (@squishyliquid)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define DYNAMIC_KEYMAP_LAYER_COUNT 8

// sizeof(he_config_t): 2 profiles * 6 rows * 13 cols * 5 + 8 pairs * 6 + 2
#define EECONFIG_KB_DATA_SIZE 830
// Increase when the meaning of a he_config_t field changes, so stored settings reset.
#define EECONFIG_KB_DATA_VERSION 1

// #define DEBUG_MATRIX_SCAN_RATE //
#define ADC_RESOLUTION ADC_CFGR1_RES_12BIT

#define SERIAL_USART_FULL_DUPLEX   // Enable full duplex operation mode.
#define SERIAL_USART_TX_PIN GP0    // USART TX pin
#define SERIAL_USART_RX_PIN GP1    // USART RX pin
#define SERIAL_USART_SPEED 2000000

#define BOOTMAGIC_ROW 0
#define BOOTMAGIC_COLUMN 3
#define BOOTMAGIC_ROW_RIGHT 3
#define BOOTMAGIC_COLUMN_RIGHT 8

#define BOOTMAGIC_ROW_2 0
#define BOOTMAGIC_COLUMN_2 4
#define BOOTMAGIC_ROW_RIGHT_2 4
#define BOOTMAGIC_COLUMN_RIGHT_2 4

#define MUX_BITS 4

#define ALL_MUX_PINS { GP18, GP19, GP20, GP21 }

#define PICO_XOSC_STARTUP_DELAY_MULTIPLIER 64

#define SPLIT_TRANSACTION_IDS_USER HE_CONFIG_SYNC
#define SPLIT_LAYER_STATE_ENABLE