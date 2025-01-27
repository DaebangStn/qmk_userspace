#include "color.h"
#include "keycodes.h"
#include "keymap_us.h"
#include "quantum_keycodes.h"
#include "raw_hid.h"
#include QMK_KEYBOARD_H
#if __has_include("keymap.h")
    #include "keymap.h"
#endif

#ifdef CONSOLE_ENABLE
    #include "print.h"
    #define DEBUG_CONSOLE(x) print(x)
#else
    #define DEBUG_CONSOLE(x)
#endif

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

// Forward declare the tap dance actions array
tap_dance_action_t tap_dance_actions[];
static bool is_hangul_mode = false;

// Define the static variables with PROGMEM
static const tap_dance_tap_hold_t PROGMEM td_quot_del = {KC_QUOT, KC_DEL, 0};
static const tap_dance_tap_hold_t PROGMEM td_bksp_f12 = {KC_BSPC, KC_F12, 0};

// Separate enum for tap dance
enum {
    TD_CONSOLE,
    TD_QW,
    TD_QUOTE_DEL,
    TD_BKSP_F12,
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
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
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
            if (is_hangul_mode) {
                DEBUG_CONSOLE("Base layer with Hangul mode\n");
                rgblight_sethsv(HSV_TURQUOISE);    // Green for base layer
            } else {
                DEBUG_CONSOLE("Base layer with English mode\n");
                rgblight_sethsv(HSV_GREEN);      // Red for other layers
            }
            break;
        case 1:
            DEBUG_CONSOLE("Layer 1\n");
            rgblight_sethsv(HSV_BLUE);     // Blue for layer 1
            break;
        default:
            DEBUG_CONSOLE("Other layer\n");
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
        case TD(TD_QUOTE_DEL):
        case TD(TD_BKSP_F12):
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
    [TD_QW] = ACTION_TAP_DANCE_TAP_HOLD(KC_Q, KC_W),
    [TD_QUOTE_DEL] = ACTION_TAP_DANCE_TAP_HOLD(KC_QUOT, KC_DEL),
    [TD_BKSP_F12] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, KC_F12),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3_ex2(
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_UP,    KC_LEFT, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_DOWN,          KC_RGHT, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_ENT,
        KC_LNG1, KC_Z, KC_X, KC_C, KC_V, KC_B,                      KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, TD(TD_QUOTE_DEL),
        LSFT_T(KC_SPC), MO(1), KC_LCTL,                                                LALT_T(KC_SPC), KC_LSFT, LCTL_T(KC_LGUI)
        ),
    [1] = LAYOUT_split_3x6_3_ex2(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_PGUP,      KC_EQUAL, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, TD(TD_BKSP_F12),
        KC_DOT, KC_1, KC_2, KC_3, KC_4, KC_5, KC_PGDN,          KC_MINS, KC_6, KC_7, KC_8, KC_9, KC_0, KC_ENT, KC_PSCR,
        KC_PSCR, KC_INS, KC_DEL, KC_HOME, KC_END,                         KC_LCBR, KC_RCBR, KC_LPRN, KC_RPRN, KC_LBRC, KC_RBRC,
        KC_LSFT, KC_NO, KC_LCTL,                                                KC_SPC, KC_LSFT, KC_LCTL
        )
};
