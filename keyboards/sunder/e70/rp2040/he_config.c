// Copyright 2023 squishyliquid (@squishyliquid)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "he_config.h"
#include "he_matrix.h"
#include "he_sync.h"
#include "eeconfig.h"
#include "quantum.h"

_Static_assert(sizeof(he_config_t) == EECONFIG_KB_DATA_SIZE, "EECONFIG_KB_DATA_SIZE must match sizeof(he_config_t)");

static void save_field(const void *field, uint32_t size) {
    uint32_t offset = (const uint8_t *)field - (const uint8_t *)&he_config;
    eeconfig_update_kb_datablock(field, offset, size);
}

// Each half reads only its own sensors, so both keys of a pair must be on the same half.
static bool is_active_pair(const input_priority_t *pair_cfg) {
    return pair_cfg->profile <= INPUT_PRIORITY_PROFILE_ALL &&
           pair_cfg->primary_row < MATRIX_ROWS && pair_cfg->primary_col < MATRIX_COLS &&
           pair_cfg->secondary_row < MATRIX_ROWS && pair_cfg->secondary_col < MATRIX_COLS &&
           pair_cfg->primary_row / ROWS_PER_HAND == pair_cfg->secondary_row / ROWS_PER_HAND &&
           (pair_cfg->primary_row != pair_cfg->secondary_row || pair_cfg->primary_col != pair_cfg->secondary_col) &&
           pair_cfg->resolution <= INPUT_PRIORITY_RESOLUTION_DEPTH;
}

static void rebuild_input_priority_indices(void) {
    memset(input_priority_indices, 0, sizeof(input_priority_indices));
    for (uint8_t index = 0; index < NUM_INPUT_PRIORITY_PAIRS; index++) {
        const input_priority_t *pair_cfg = &he_config.input_priority_pairs[index];
        if (!is_active_pair(pair_cfg)) {
            continue;
        }
        for (uint8_t profile = 0; profile < ACTUATION_PROFILE_COUNT; profile++) {
            if (pair_cfg->profile == profile || pair_cfg->profile == INPUT_PRIORITY_PROFILE_ALL) {
                input_priority_indices[profile][pair_cfg->primary_row][pair_cfg->primary_col]     = index + 1;
                input_priority_indices[profile][pair_cfg->secondary_row][pair_cfg->secondary_col] = index + 1;
            }
        }
    }
}

// The master forwards each change so that the slave half stores the same settings.
static bool should_forward(void) {
    if (!is_keyboard_master()) {
        return false;
    }
    if (!get_synced_status()) {
        enable_full_sync();
    }
    return true;
}

void he_config_load(void) {
    eeconfig_read_kb_datablock(&he_config, 0, sizeof(he_config));
    rebuild_input_priority_indices();
}

void eeconfig_init_kb_datablock(void) {
    he_config = he_config_default;
    eeconfig_update_kb_datablock(&he_config, 0, sizeof(he_config));
    rebuild_input_priority_indices();
    request_full_sync();
}

bool he_config_set_actuation(uint8_t profile, uint8_t row, uint8_t col, const actuation_t *actuation_cfg) {
    if (profile >= ACTUATION_PROFILE_COUNT || row >= MATRIX_ROWS || col >= MATRIX_COLS) {
        return false;
    }
    if (actuation_cfg->actuation_point < MIN_ACTUATION || actuation_cfg->rt_mode > MAX_RT_MODE) {
        return false;
    }
    // A release point at or deeper than the actuation point would release a pressed key.
    if (actuation_cfg->reset_point != DEFAULT_RESET && (actuation_cfg->reset_point >= actuation_cfg->actuation_point || actuation_cfg->reset_point < MIN_RESET_POINT)) {
        return false;
    }
    actuation_t *field = &he_config.actuation_matrix[profile][row][col];
    *field             = *actuation_cfg;
    save_field(field, sizeof(*field));
    if (should_forward()) {
        he_sync_actuation(profile, row, col);
    }
    return true;
}

bool he_config_set_input_priority_pair(uint8_t index, const input_priority_t *pair_cfg) {
    if (index >= NUM_INPUT_PRIORITY_PAIRS) {
        return false;
    }
    if (pair_cfg->profile != INPUT_PRIORITY_PAIR_DISABLED && !is_active_pair(pair_cfg)) {
        return false;
    }
    input_priority_t *field = &he_config.input_priority_pairs[index];
    *field                  = *pair_cfg;
    rebuild_input_priority_indices();
    save_field(field, sizeof(*field));
    if (should_forward()) {
        he_sync_input_priority_pair(index);
    }
    return true;
}

bool he_config_set_switch(uint8_t switch_option) {
    if (switch_option > SWITCH_390) {
        return false;
    }
    if (he_config.switch_option != switch_option) {
        enable_he_sensor_reinit();
    }
    he_config.switch_option = switch_option;
    save_field(&he_config.switch_option, sizeof(he_config.switch_option));
    if (should_forward()) {
        he_sync_switch();
    }
    return true;
}

bool he_config_set_special_layer(uint8_t layer_index) {
    if (layer_index > DYNAMIC_KEYMAP_LAYER_COUNT) {
        return false;
    }
    he_config.special_layer = layer_index;
    save_field(&he_config.special_layer, sizeof(he_config.special_layer));
    if (should_forward()) {
        he_sync_special_layer();
    }
    return true;
}
