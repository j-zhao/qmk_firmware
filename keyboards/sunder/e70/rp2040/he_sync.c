// Copyright 2023 squishyliquid (@squishyliquid)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "he_sync.h"
#include "he_config.h"
#include "quantum.h"
#include "transactions.h"
#include "split_util.h"

void he_sync_actuation(uint8_t profile, uint8_t row, uint8_t col) {
    const actuation_t *actuation_cfg = &he_config.actuation_matrix[profile][row][col];
    uint8_t cfg[] = {
        ACTUATION_SYNC,
        profile,
        row,
        col,
        actuation_cfg->actuation_point,
        actuation_cfg->reset_point,
        actuation_cfg->rt_mode,
        actuation_cfg->rt_press,
        actuation_cfg->rt_release
    };
    transaction_rpc_send(HE_CONFIG_SYNC, sizeof(cfg), cfg);
}

void he_sync_input_priority_pair(uint8_t index) {
    const input_priority_t *pair_cfg = &he_config.input_priority_pairs[index];
    uint8_t cfg[] = {
        INPUT_PRIORITY_SYNC,
        index,
        pair_cfg->layer,
        pair_cfg->primary_row,
        pair_cfg->primary_col,
        pair_cfg->secondary_row,
        pair_cfg->secondary_col,
        pair_cfg->resolution
    };
    transaction_rpc_send(HE_CONFIG_SYNC, sizeof(cfg), cfg);
}

void he_sync_switch(void) {
    uint8_t cfg[] = {
        SWITCH_OPTION_SYNC,
        he_config.switch_option
    };
    transaction_rpc_send(HE_CONFIG_SYNC, sizeof(cfg), cfg);
}

void he_sync_special_layer(void) {
    uint8_t cfg[] = {
        SPECIAL_LAYER_SYNC,
        he_config.special_layer
    };
    transaction_rpc_send(HE_CONFIG_SYNC, sizeof(cfg), cfg);
}

void he_full_sync(void) {
    for (int profile = 0; profile < ACTUATION_PROFILE_COUNT; profile ++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            for (int col = 0; col < MATRIX_COLS; col++) {
                he_sync_actuation(profile, row, col);
            }
        }
    }

    for (int pair_index = 0; pair_index < NUM_INPUT_PRIORITY_PAIRS; pair_index ++) {
        he_sync_input_priority_pair(pair_index);
    }

    he_sync_switch();
    he_sync_special_layer();
}

void he_sync_slave(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    const uint8_t* data = (const uint8_t*)in_data;

    switch (data[0]) {
    case ACTUATION_SYNC: {
        uint8_t profile = data[1];
        uint8_t row = data[2];
        uint8_t col = data[3];

        actuation_t actuation_cfg = {
            .actuation_point = data[4],
            .reset_point = data[5],
            .rt_mode = data[6],
            .rt_press = data[7],
            .rt_release = data[8]
        };

        he_config_set_actuation(profile, row, col, &actuation_cfg);
        break;
    }
    case INPUT_PRIORITY_SYNC: {
        uint8_t index = data[1];

        input_priority_t pair_cfg = {
            .layer = data[2],
            .primary_row = data[3],
            .primary_col = data[4],
            .secondary_row = data[5],
            .secondary_col = data[6],
            .resolution = data[7]
        };

        he_config_set_input_priority_pair(index, &pair_cfg);
        break;
    }
    case SWITCH_OPTION_SYNC:
        he_config_set_switch(data[1]);
        break;
    case SPECIAL_LAYER_SYNC:
        he_config_set_special_layer(data[1]);
        break;
    default:
        break;
    }
}

void keyboard_post_init_kb(void) {
    he_config_load();
    transaction_register_rpc(HE_CONFIG_SYNC, he_sync_slave);
    keyboard_post_init_user();
}
