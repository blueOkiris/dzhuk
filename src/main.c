// Entry point for Dzhuk application code

#include <ssd1306.h>
#include <main_menu.h>

int main(int argc, char **argv) {
    ssd1306__init();

    while (1) {
        app_id_t app = main_menu();
        switch (app) {
            default:
                break;
        }
    }

    ssd1306__deinit();
    return 0;
}
