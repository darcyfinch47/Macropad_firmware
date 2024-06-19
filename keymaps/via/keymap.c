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
uint8_t volume = 0; // Variable to store received number
uint8_t cpu = 0; // Variable to store received number
uint8_t memory = 0; // Variable to store received number
bool rotation = false;

bool via_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id = &(data[0]);
    is_hid_connected = true;

    switch (*command_id) {
        case 1:
            volume = data[1];
            break;
        case 2:
            cpu = data[1];
            break;
        case 3:
            memory = data[1];
            break;
        case 4:
            snprintf(screen_data_buffer, sizeof(screen_data_buffer), "%s", (char *)&data[1]);
            break;
    }
    return false;
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { 
    if (rotation) {
        return OLED_ROTATION_270; 
    } else {
        return OLED_ROTATION_180; 
    }
}

// Used to draw on to the oled screen
bool oled_task_user() {            
    oled_set_cursor(0, 0);
    if (is_hid_connected) {
        char volume_str[16];
        snprintf(volume_str, sizeof(volume_str), "%u", volume);
        char cpu_str[16];
        snprintf(cpu_str, sizeof(cpu_str), "%u", cpu);
        char memory_str[16];
        snprintf(memory_str, sizeof(memory_str), "%u", memory);
        
        if (rotation) {
            oled_write_ln("Link!", true);

            // Display the Volume
            oled_set_cursor(0, 2);
            oled_write_ln("VOL:", false);
            oled_set_cursor(3, 3);
            oled_write_ln(volume_str, false);
            
            oled_set_cursor(0, 5);
            oled_write_ln("CPU:", false);
            oled_set_cursor(3, 6);
            oled_write_ln(cpu_str, false);

            oled_set_cursor(0, 8);
            oled_write_ln("MEM:", false);
            oled_set_cursor(3, 9);
            oled_write_ln(memory_str, false);

            oled_set_cursor(0, 15);
            oled_write_ln(screen_data_buffer, false);
        } else {
            oled_write_ln("Linked!", true);
            // Display the Volume
            oled_set_cursor(0, 2);
            oled_write_ln("VOL:  CPU:  MEM:", false);
            // Buffer to hold the formatted string
            char buffer[100]; // Adjust size as needed
            
            // Format the string using sprintf
            sprintf(buffer, "   %d     %d     %d", volume, cpu, memory);
            oled_write_ln(buffer, false);
            
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