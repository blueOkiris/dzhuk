// Implement the menu

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <font8x8_basic.h>
#include <ssd1306.h>
#include <main_menu.h>

// --------- Private Data --------

static const uint8_t SPR_LOCAL[4][8] = {
    {
        // Bottom-left
        0b11111111,
        0b11111111,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11001100,
        0b11011110,
        0b11011110
    }, {
        // Bottom-right
        0b11001110,
        0b11000011,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11000000,
        0b11111111,
        0b11111111
    }, {
        // Top-left
        0b11110000,
        0b11111000,
        0b11001100,
        0b11000110,
        0b11111111,
        0b00000011,
        0b00000011,
        0b00000011
    }, {
        // Top-right
        0b00000011,
        0b11111011,
        0b00010011,
        0b00100011,
        0b00000011,
        0b00000011,
        0b11111111,
        0b11111111
    }
};
static const uint8_t SPR_SPOTIFY[4][8] = {
    {
        // Bottom-left
        0b00000111,
        0b00011000,
        0b00100000,
        0b01000010,
        0b01000001,
        0b10001001,
        0b10000100,
        0b10000100
    }, {
        // Bottom-right
        0b10000100,
        0b10000100,
        0b10001001,
        0b01000001,
        0b01000010,
        0b00100000,
        0b00011000,
        0b00000111
    }, {
        // Top-left
        0b11100000,
        0b00011000,
        0b01000100,
        0b00100010,
        0b00100010,
        0b00010001,
        0b10010001,
        0b10010001
    }, {
        // Top-right
        0b10010001,
        0b10010001,
        0b00010001,
        0b00100010,
        0b00100010,
        0b01000100,
        0b00011000,
        0b11100000
    }
};
static const uint8_t SPR_CONFIG[4][8] = {
    {
        // Bottom-left
        0b01100011,
        0b01100011,
        0b01100011,
        0b01100011,
        0b01100011,
        0b01100011,
        0b01100011,
        0b01100111
    }, {
        // Bottom-right
        0b01100111,
        0b01100011,
        0b01100011,
        0b01100011,
        0b01100011,
        0b11110011,
        0b11110011,
        0b01100011
    }, {
        // Top-left
        0b00011000,
        0b00111100,
        0b00111100,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b10011000
    }, {
        // Top-right
        0b10011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000
    }
};

// --------- Public API ---------

app_id_t main_menu(void) {
    ssd1306__fill(true);

    ssd1306__copy_tile(SPR_ASCII[(size_t) '1'], 1, 1);
    ssd1306__copy_tile(SPR_LOCAL[0], 4, 1);
    ssd1306__copy_tile(SPR_LOCAL[1], 5, 1);
    ssd1306__copy_tile(SPR_LOCAL[2], 4, 0);
    ssd1306__copy_tile(SPR_LOCAL[3], 5, 0);
    const char *const local_msg = "Storage";
    const size_t local_msg_len = strlen(local_msg);
    for (size_t i = 0; i < local_msg_len; i++) {
        ssd1306__copy_tile(SPR_ASCII[(size_t) local_msg[i]], i, 3);
    }

    ssd1306__copy_tile(SPR_ASCII[(size_t) '3'], 14, 1);
    ssd1306__copy_tile(SPR_SPOTIFY[0], 10, 1);
    ssd1306__copy_tile(SPR_SPOTIFY[1], 11, 1);
    ssd1306__copy_tile(SPR_SPOTIFY[2], 10, 0);
    ssd1306__copy_tile(SPR_SPOTIFY[3], 11, 0);
    char *spotify_msg = "Spotify";
    const size_t spotify_msg_len = strlen(spotify_msg);
    for (size_t i = 0; i < spotify_msg_len; i++) {
        ssd1306__copy_tile(SPR_ASCII[(size_t) spotify_msg[i]], 9 + i, 3);
    }

    ssd1306__copy_tile(SPR_ASCII[(size_t) '2'], 5, 5);
    ssd1306__copy_tile(SPR_CONFIG[0], 8, 5);
    ssd1306__copy_tile(SPR_CONFIG[1], 9, 5);
    ssd1306__copy_tile(SPR_CONFIG[2], 8, 4);
    ssd1306__copy_tile(SPR_CONFIG[3], 9, 4);
    char *settings_msg = "Settings";
    const size_t settings_msg_len = strlen(settings_msg);
    for (size_t i = 0; i < settings_msg_len; i++) {
        ssd1306__copy_tile(SPR_ASCII[(size_t) settings_msg[i]], 4 + i, 7);
    }

    ssd1306__dispatch();

    while(1) {}
    return APPID_SETTINGS;
}
