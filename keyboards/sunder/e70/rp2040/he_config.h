// Copyright 2023 squishyliquid (@squishyliquid)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "he_keys.h"

void he_config_load(void);

bool he_config_set_actuation(uint8_t profile, uint8_t row, uint8_t col, const actuation_t *actuation_cfg);

bool he_config_set_input_priority_pair(uint8_t index, const input_priority_t *pair_cfg);

bool he_config_set_switch(uint8_t switch_option);

bool he_config_set_special_layer(uint8_t layer_index);
