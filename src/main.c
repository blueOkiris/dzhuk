// Entry point for Dzhuk application code

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <font8x8_basic.h>
#include <ssd1306.h>

// Wher to go after selecting an option on the main menu
typedef enum {
    APPID_MUSIC_PLAYER,
    APPID_SPOTIFY,
    APPID_SETTINGS
} app_id_t;

app_id_t menu_main(void);

int main(int argc, char **argv) {
    ssd1306__init();

    while (1) {
        app_id_t app = menu_main();
        switch (app) {
            default:
                break;
        }
    }

    ssd1306__deinit();
    return 0;
}

app_id_t menu_main(void) {
    ssd1306__fill(true);
    const char *opts[3] = {
        "1. Player",
        "2. Spotify",
        "3. Settings"
    };
    const size_t x_pad = (16 - strlen(opts[2])) / 2;
    const size_t y_pad = (8 - 3 * 2) / 2;
    for (int i = 0; i < 3; i++) {
        size_t len = strlen(opts[i]);
        for (size_t j = 0; j < len; j++) {
            ssd1306__copy_tile(
                SPR_ASCII[(size_t) opts[i][j]], x_pad + j, y_pad + i * 2
            );
        }
    }
    ssd1306__dispatch();

    while(1) {}
    return APPID_SETTINGS;
}
