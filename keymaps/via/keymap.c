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
        KC_1,    KC_2,    KC_3,
        KC_4,    KC_5,    KC_6,
        KC_7,    KC_8,    KC_9,
        KC_MUTE
    )
};

// HID input
bool is_hid_connected = false; // Flag indicating if we have a PC connection yet
char screen_data_buffer[64] = {0}; // Buffer to store received text
uint8_t received_number = 0; // Variable to store received number

bool via_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id = &(data[0]);

    switch (*command_id) {
        case 1:
            is_hid_connected = true;
            received_number = data[1];
            break;
        case 2:
            is_hid_connected = true;
            snprintf(screen_data_buffer, sizeof(screen_data_buffer), "%s", (char *)&data[1]);
            break;
    }
    return false;
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { 
    return OLED_ROTATION_270; 
}

// Used to draw on to the oled screen
bool oled_task_user() {            
    oled_set_cursor(0, 0);
    if (is_hid_connected) {
        oled_write_ln("Link!", true);

        // Display the received number
        oled_set_cursor(0, 2);
        char number_str[16];
        if (received_number == 100) {
            snprintf(number_str, sizeof(number_str), "V:%u", received_number);
        } else if (received_number < 10){
            snprintf(number_str, sizeof(number_str), "V:  %u", received_number);
        } else {
            snprintf(number_str, sizeof(number_str), "V: %u", received_number);
        }
        oled_write_ln(number_str, false);

        oled_set_cursor(0, 4);
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