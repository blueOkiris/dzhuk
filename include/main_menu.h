// Draw a menu and receive input to determine which app to run

#pragma once

// Where to go after selecting an option on the main menu
typedef enum {
    APPID_MUSIC_PLAYER = 0,
    APPID_SPOTIFY,
    APPID_SETTINGS
} app_id_t;

app_id_t main_menu(void);
