// Driver for a 128x64 SSD1306 OLED I2C board

#pragma once

#include <stdbool.h>
#include <stdint.h>

/*
 * Configure the I2C and display
 *
 * While this sets up basic stuff like turning on display and setting voltages,
 * it also configures data order.
 *
 * So first of all, we go row by row. There are 8 rows of height 8px
 * As we go left to right through the row, we fill each 8px column.
 * We can change the start row and col and the max col and row, but it still works the same,
 * just different limits.
 *
 * That means each sprite is made up of 8 vertical bytes,
 * with b0 being the top and b7 the bottom in each one
 */
void ssd1306__init(void);

// Fill the entire display buffer (either black, is_clear = 1, or white, is_clear = 0)
void ssd1306__fill(bool is_clear);

// Copy tile data into buffer
void ssd1306__copy_tile(const uint8_t tile[8], uint8_t col, uint8_t row);

// Transfer the display buffer to the OLED
void ssd1306__dispatch(void);

// Update only a subset of tiles (for performance).
// It does have a copy of tiles into temp buff tho, so only efficient at small width/height
void ssd1306__copy_and_partial_dispatch(
    uint8_t col, uint8_t row, uint8_t num_cols, uint8_t num_rows
);

// Draw an image offset from the tile map. Expensive, so only used for sprites
void ssd1306__spr_dispatch(const uint8_t spr[8], uint8_t x, uint8_t y);

// Draw over where a sprite was with the background (call to partial dispatch under the hood)
void ssd1306__clear_spr_dispatch(uint8_t x, uint8_t y);

// Shift the whole screen up/down
void ssd1306__vscroll(const uint8_t rows);

// Cleanup SSD1306 (mainly just close I2C)
void ssd1306__deinit(void);

// A simple test of the display
void ssd1306__test(void);
