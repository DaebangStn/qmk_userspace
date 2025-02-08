#include "color.h"
#include "keycodes.h"
#include "keymap_us.h"
#include "process_tap_dance.h"
#include "quantum_keycodes.h"
#include "raw_hid.h"
#include QMK_KEYBOARD_H
#if __has_include("keymap.h")
    #include "keymap.h"
#endif

#ifdef CONSOLE_ENABLE
    #include "print.h"
    #define DEBUG_CONSOLE(...) xprintf(__VA_ARGS__)
#else
    #define DEBUG_CONSOLE(...)
#endif

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

// Forward declare the tap dance actions array
tap_dance_action_t tap_dance_actions[];
static bool is_hangul_mode = false;

// Separate enum for tap dance keys, TH: tap and hold, HDTH: hold and double tap hold
enum {
    TD_CONSOLE,
    TD_TH_QW,
    TD_TH_F1_TILDE,
    TD_TH_QUOTE_DEL,
    TD_TH_BKSP_F12,
    TD_TH_ESC_TG1,
    TD_TH_HANGUL_ALT,
};

// Separate enum for custom keycodes (if needed)
enum custom_keycodes {
    CUSTOM_START = SAFE_RANGE
    // Add custom keycodes here
};

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1 && !state->interrupted) {
            DEBUG_CONSOLE("hold key: 0x%x\n", tap_hold->hold);
            // Check if the hold keycode is a layer toggle
            if ((tap_hold->hold & QK_TOGGLE_LAYER) == QK_TOGGLE_LAYER) {
                uint8_t layer = (tap_hold->hold) & 0x0F;
                DEBUG_CONSOLE("toggle layer: 0x%x\n", layer);
                layer_invert(layer);
            } else if ((tap_hold->hold & QK_TO) == QK_TO) {
                uint8_t layer = (tap_hold->hold) & 0x0F;
                DEBUG_CONSOLE("activate layer: 0x%x\n", layer);
                layer_move(layer);
            } else {
                register_code16(tap_hold->hold);
                tap_hold->held = tap_hold->hold;
            }
        } else {
            DEBUG_CONSOLE("tap key: 0x%x\n", tap_hold->tap);
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;
    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case 0:
            DEBUG_CONSOLE("[Layer0] QWERTY\n");
            rgblight_sethsv(HSV_GREEN);      // Red for other layers
            break;
        case 1:
            DEBUG_CONSOLE("[Layer1] Colemak\n");
            rgblight_sethsv(HSV_PURPLE);     // Red for layer 2
            break;
        case 2:
            DEBUG_CONSOLE("[Layer2] Symbols\n");
            rgblight_sethsv(HSV_BLUE);     // Blue for layer 1
            break;
        default:
            DEBUG_CONSOLE("[Layer] Other\n");
            rgblight_sethsv(HSV_RED);      // Red for other layers
            break;
    }
    return state;
}

void keyboard_post_init_user(void) {
    rgblight_enable();
    layer_state_set_user(layer_state);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;

    switch (keycode) {
        case TD(TD_TH_QW):
        case TD(TD_TH_F1_TILDE):
        case TD(TD_TH_QUOTE_DEL):
        case TD(TD_TH_BKSP_F12):
        case TD(TD_TH_ESC_TG1):
        case TD(TD_TH_HANGUL_ALT):
            action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
    }
    return true;
}

void debug_tap_dance(tap_dance_state_t *state, void *user_data) {
    DEBUG_CONSOLE("tap dance key pressed\n");
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    // Assume the first byte of data represents the state
    if (length >= 1) {
        switch (data[0]) {
            case 1:  // Hangul mode
                DEBUG_CONSOLE("Hangul mode\n");
                is_hangul_mode = true;
                break;
            case 0:  // English mode
                DEBUG_CONSOLE("English mode\n");
                is_hangul_mode = false;
                break;
            default:
                DEBUG_CONSOLE("Unknown mode\n");
                break;  // Handle additional states if necessary
        }
        layer_state_set_user(layer_state);
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_CONSOLE] = ACTION_TAP_DANCE_FN(debug_tap_dance),
    [TD_TH_QW] = ACTION_TAP_DANCE_TAP_HOLD(KC_Q, KC_W),
    [TD_TH_F1_TILDE] = ACTION_TAP_DANCE_TAP_HOLD(KC_TILD, KC_F1),
    [TD_TH_QUOTE_DEL] = ACTION_TAP_DANCE_TAP_HOLD(KC_QUOT, KC_DEL),
    [TD_TH_BKSP_F12] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, KC_F12),
    [TD_TH_ESC_TG1] = ACTION_TAP_DANCE_TAP_HOLD(KC_ESC, TG(1)),
    [TD_TH_HANGUL_ALT] = ACTION_TAP_DANCE_TAP_HOLD(KC_LNG1, KC_LALT),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3_ex2(
        TD(TD_TH_ESC_TG1), KC_Q, KC_W, KC_E, KC_R, KC_T, KC_UP,    KC_LEFT, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_DOWN,          KC_RGHT, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_ENT,
        TD(TD_TH_HANGUL_ALT), KC_Z, KC_X, KC_C, KC_V, KC_B,                      KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, TD(TD_TH_QUOTE_DEL),
        LSFT_T(KC_SPC), MO(2), KC_LCTL,                                                LALT_T(KC_SPC), KC_LSFT, LCTL_T(KC_LGUI)
        ),
    [1] = LAYOUT_split_3x6_3_ex2(
        _______, KC_Q, KC_W, KC_F, KC_P, KC_B, _______,       _______, KC_J, KC_L, KC_U, KC_Y, KC_SCLN, _______,
        _______, KC_A, KC_R, KC_S, KC_T, KC_G, _______,     _______, KC_M, KC_N, KC_E, KC_I, KC_O, _______,
        _______, KC_Z, KC_X, KC_C, KC_D, KC_V,      KC_K, KC_H, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
        ),
    [2] = LAYOUT_split_3x6_3_ex2(
        TD(TD_TH_F1_TILDE), KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_PGUP,      KC_HOME, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, TD(TD_TH_BKSP_F12),
        KC_DOT, KC_1, KC_2, KC_3, KC_4, KC_5, KC_PGDN,          KC_END, KC_6, KC_7, KC_8, KC_9, KC_0, _______,
        KC_PSCR, KC_BSLS, KC_INS, KC_DEL, KC_EQUAL, KC_MINS,                          KC_LCBR, KC_RCBR, KC_LPRN, KC_RPRN, KC_LBRC, KC_RBRC,
        _______, _______, _______, _______, _______, _______
        ),
};
