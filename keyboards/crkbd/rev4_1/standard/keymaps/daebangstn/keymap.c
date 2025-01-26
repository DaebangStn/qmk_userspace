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

// Separate enum for tap dance
enum {
    TD_CONSOLE,
    TD_QUOTE_DEL,
};

// Separate enum for custom keycodes (if needed)
enum custom_keycodes {
    CUSTOM_START = SAFE_RANGE
    // Add custom keycodes here
};


void debug_tap_dance(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        DEBUG_CONSOLE("Single tap\n");
    } else if (state->count == 2) {
        DEBUG_CONSOLE("Double tap\n");
    }
}

void quot_del_tap(tap_dance_state_t *state, void *user_data) {
    if (state->pressed) {
        tap_code(KC_QUOT);
    } else {
        tap_code(KC_DEL);
    }
}


tap_dance_action_t tap_dance_actions[] = {
    [TD_CONSOLE] = ACTION_TAP_DANCE_FN(debug_tap_dance),
    [TD_QUOTE_DEL] = ACTION_TAP_DANCE_FN(quot_del_tap),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3_ex2(
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_UP,            KC_LEFT, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_DOWN,         KC_RGHT, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_ENT,
        KC_LNG1, KC_Z, KC_X, KC_C, KC_V, KC_B,                  KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, TD(TD_QUOTE_DEL),
        KC_LCTL, MO(1), KC_LCTL,                                LALT_T(TD_CONSOLE), KC_LSFT, KC_LGUI
        ),
    [1] = LAYOUT_split_3x6_3_ex2(
        KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_PGUP,      KC_LEFT, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_BSPC,
        KC_DOT, KC_1, KC_2, KC_3, KC_4, KC_5, KC_PGDN,          KC_RGHT, KC_6, KC_7, KC_8, KC_9, KC_0, KC_ENT, KC_PSCR,
        KC_NO, KC_INS, KC_DEL, KC_HOME, KC_END,                 KC_LCBR, KC_RCBR, KC_LPRN, KC_RPRN, KC_LBRC, KC_RBRC,
        KC_NO, KC_NO, KC_LCTL,                                  KC_SPC, KC_NO, KC_NO
        )
};

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case 0:
            DEBUG_CONSOLE("Base layer\n");
            rgblight_sethsv(HSV_GREEN);    // Green for base layer
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
    layer_state_set_user(get_highest_layer(layer_state));
}
