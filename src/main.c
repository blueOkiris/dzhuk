// Entry point for Dzhuk application code

#include <stdbool.h>
#include <ssd1306.h>

int main(int argc, char **argv) {
    ssd1306__init();
    ssd1306__test();
    ssd1306__deinit();
    return 0;
}
