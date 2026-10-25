// Copyright 2023 squishyliquid (@squishyliquid)
// SPDX-License-Identifier: GPL-2.0-or-later

#ifdef VIA_ENABLE

#    include "he_config.h"
#    include "via.h"

#    define HE_FIRMWARE_VERSION 0

// data = [ command_id, id_custom_channel, value_id, args..., value... ]
enum he_via_value_id {
    id_he_actuation = 1,
    id_he_input_priority_pair,
    id_he_switch,
    id_he_special_layer,
    id_he_version,
};

static bool he_via_get(uint8_t value_id, uint8_t *args) {
    switch (value_id) {
        case id_he_actuation:
            if (args[0] >= ACTUATION_PROFILE_COUNT || args[1] >= MATRIX_ROWS || args[2] >= MATRIX_COLS) {
                return false;
            }
            memcpy(&args[3], &he_config.actuation_matrix[args[0]][args[1]][args[2]], sizeof(actuation_t));
            return true;
        case id_he_input_priority_pair:
            if (args[0] >= NUM_INPUT_PRIORITY_PAIRS) {
                return false;
            }
            memcpy(&args[1], &he_config.input_priority_pairs[args[0]], sizeof(input_priority_t));
            return true;
        case id_he_switch:
            args[0] = he_config.switch_option;
            return true;
        case id_he_special_layer:
            args[0] = he_config.special_layer;
            return true;
        case id_he_version:
            args[0] = HE_FIRMWARE_VERSION;
            return true;
        default:
            return false;
    }
}

static bool he_via_set(uint8_t value_id, uint8_t *args) {
    switch (value_id) {
        case id_he_actuation: {
            actuation_t actuation_cfg;
            memcpy(&actuation_cfg, &args[3], sizeof(actuation_cfg));
            return he_config_set_actuation(args[0], args[1], args[2], &actuation_cfg);
        }
        case id_he_input_priority_pair: {
            input_priority_t pair_cfg;
            memcpy(&pair_cfg, &args[1], sizeof(pair_cfg));
            return he_config_set_input_priority_pair(args[0], &pair_cfg);
        }
        case id_he_switch:
            return he_config_set_switch(args[0]);
        case id_he_special_layer:
            return he_config_set_special_layer(args[0]);
        default:
            return false;
    }
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id = &data[0];
    uint8_t  channel_id = data[1];
    uint8_t  value_id   = data[2];
    uint8_t *args       = &data[3];

    if (channel_id != id_custom_channel) {
        *command_id = id_unhandled;
        return;
    }

    bool handled;
    switch (*command_id) {
        case id_custom_get_value:
            handled = he_via_get(value_id, args);
            break;
        case id_custom_set_value:
            handled = he_via_set(value_id, args);
            break;
        case id_custom_save:
            // Each set writes to EEPROM immediately.
            handled = true;
            break;
        default:
            handled = false;
            break;
    }

    if (!handled) {
        *command_id = id_unhandled;
    }
}

#endif
