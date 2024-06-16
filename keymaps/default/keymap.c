// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include <stdbool.h>
#include "quantum.h"
#include "raw_hid.h"

bool encoder_update_user(uint8_t index, bool clockwise) {
    oled_on();
    if (index == 0) { 
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    return false;
}

// Global variable to store the last keycode pressed
static uint16_t last_keycode = KC_NO;

// Key processing function
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        last_keycode = keycode;
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_3x3x1(
        KC_7,    KC_4,    KC_1,
        KC_8,    KC_5,    KC_2,
        KC_9,    KC_6,    KC_3,
        KC_MUTE
    )
};

// HID input
bool is_hid_connected = false; // Flag indicating if we have a PC connection yet
char screen_data_buffer[64] = {0}; // Buffer to store received text

void raw_hid_receive(uint8_t *data, uint8_t length) {
    is_hid_connected = true;
    if (length > 1 && data[0] == 1) {
        snprintf(screen_data_buffer, sizeof(screen_data_buffer), "%s", (char *)&data[1]);
        return;
    }
    raw_hid_send(data, length);
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { 
    return OLED_ROTATION_270; 
}

// Used to draw on to the oled screen
bool oled_task_user() {            
    oled_set_cursor(0, 0);
    if (is_hid_connected) {
        oled_write_ln("Set!", true);
        oled_write_ln(screen_data_buffer, false);
    } else {
        oled_write_ln("Hi!", false);
    }

    // oled_set_cursor(0, 2); // Adjust the cursor position as needed
    // oled_write_ln_P(PSTR("Keycode:"), false);
    // char keycode_str[16];
    // snprintf(keycode_str, sizeof(keycode_str), "0x%04X", last_keycode);
    // oled_write_ln(keycode_str, false);
    return true;
}
#endif