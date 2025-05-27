// Implement driver functionality for SSD1306 128x64 OLED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <ssd1306.h>

// -------- Private Defs --------

// Oled driver commands
#define SSD_CMD_SET_CONTRAST    0x81
#define SSD_CMD_OUTPUT_IS_RAM   0xA4
#define SSD_CMD_OUTPUT_NOT_RAM  0xA5
#define SSD_CMD_DISP_NORMAL     0xA6
#define SSD_CMD_DISP_INVERSE    0xA7
#define SSD_CMD_DISP_OFF        0xAE
#define SSD_CMD_DISP_ON         0xAF
#define SSD_CMD_SET_CLK_DIV     0xD5
#define SSD_CMD_SET_MULTIPLEX   0xA8
#define SSD_CMD_SET_DISP_OFFSET 0xD3
#define SSD_CMD_SET_START_LN    0x40
#define SSD_CMD_CHARGE_PUMP     0x8D
#define SSD_CMD_MEMORYMODE      0x20
#define SSD_CMD_SEGREMAP        0xA0
#define SSD_CMD_COMSCAN_DEC     0xC8
#define SSD_CMD_SET_COMPINS     0xDA
#define SSD_CMD_SET_PRECHARGE   0xD9
#define SSD_CMD_SET_VCOM_DETECT 0xDB
#define SSD_CMD_SET_COL_ADDR    0x21
#define SSD_CMD_SET_PAGE_ADDR   0x22

// Oled settings
#define SSD_SCREEN_WIDTH        128
#define SSD_SCREEN_HEIGHT       64
#define SSD_DISP_BUFF_SIZE      (SSD_SCREEN_WIDTH * SSD_SCREEN_HEIGHT / 8)
#define SSD_CTRL                0x00
#define SSD_DATA                0x40
#define SSD_I2C_ADDR            0x3C
#define SSD_RESET               15

// I2C info
#define I2C_FNAME               "/dev/i2c-1"

// -------- Private Data --------

static const uint8_t I2C_MSG_INIT[31] = {
    SSD_CMD_DISP_OFF,
    SSD_CMD_SET_CLK_DIV, 0xF0,                      // Suggested ratio of 0x80
    SSD_CMD_SET_MULTIPLEX, SSD_SCREEN_HEIGHT - 1,   // 1/64 duty
    SSD_CMD_SET_DISP_OFFSET, 0x00,                  // No vertical offset
    SSD_CMD_SET_START_LN | 0x00,                    // Start at line 0
    SSD_CMD_CHARGE_PUMP, 0x14,                      // Enable charge pump
    SSD_CMD_MEMORYMODE, 0x00,                       // Horizontal addressing mode
    SSD_CMD_SEGREMAP | 0x01,                        // Flip X axis (so it's left to right)
    SSD_CMD_COMSCAN_DEC,                            // Flip Y axis
    SSD_CMD_SET_COMPINS, 0x12,                      // For 128x64
    SSD_CMD_SET_CONTRAST, 0xCF,                     // Medium-high contrast
    SSD_CMD_SET_PRECHARGE, 0xF1,                    // Pre-charge timing
    SSD_CMD_SET_VCOM_DETECT, 0x40,                  // VCOM detect level
    SSD_CMD_OUTPUT_IS_RAM,                          // Resume from RAM
    SSD_CMD_DISP_NORMAL,                            // Normal display (not inverted)
    SSD_CMD_DISP_ON
};

static int I2C_FILE = 0;
static uint8_t DISP_BUFF[1 + SSD_DISP_BUFF_SIZE] = { 0 };

static const uint8_t SPR_A_TEST[8] = {
    0b00000000,
    0b11111100,
    0b00010010,
    0b00010001,
    0b00010001,
    0b00010010,
    0b11111100,
    0b00000000
};
static const uint8_t TILE_QUAD_TEST[4][8] = {
    {
        0b11111111,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001
    }, {
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b00000001,
        0b11111111
    }, {
        0b11111111,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000
    }, {
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b11111111
    }
};

// -------- Private Fn Protos --------

static bool open_i2c(void);
static void i2c_cmd(uint8_t cmd);

// -------- Public API --------

void ssd1306__init(void) {
    if (!open_i2c()) {
        return; // Screen failure due to I2C failure
    }
    for (int i = 0; i < 31; i++) {
        i2c_cmd(I2C_MSG_INIT[i]);
    }
    ssd1306__fill(true);
    ssd1306__dispatch();
}

void ssd1306__fill(bool is_clear) {
    memset(DISP_BUFF + 1, is_clear ? 0x00 : 0xFF, SSD_DISP_BUFF_SIZE);
}

void ssd1306__copy_tile(const uint8_t tile[8], uint8_t col, uint8_t row) {
    const size_t disp_buff_idx = 1 + row * SSD_SCREEN_WIDTH + (col << 3);
    memcpy(DISP_BUFF + disp_buff_idx, tile, 8);
}

void ssd1306__dispatch(void) {
    const uint8_t set_limits[8] = {
        SSD_CTRL, SSD_CMD_SET_PAGE_ADDR, 0, (SSD_SCREEN_HEIGHT >> 3) - 1,
        SSD_CTRL, SSD_CMD_SET_COL_ADDR, 0, SSD_SCREEN_WIDTH - 1
    };
    if (write(I2C_FILE, set_limits, 8) != 8) {
        perror("SSD1306 - Warning. Failed to set dispatch limits");
    }
    DISP_BUFF[0] = SSD_DATA;
    if (write(I2C_FILE, DISP_BUFF, 1 + SSD_DISP_BUFF_SIZE) != 1 + SSD_DISP_BUFF_SIZE) {
        perror("SSD1306 - Warning. Failed to dispatch display buffer");
    }
}

void ssd1306__copy_and_partial_dispatch(
        uint8_t col, uint8_t row, uint8_t num_cols, uint8_t num_rows) {
    // Fill buffer
    uint8_t tile_buff[1 + num_cols * num_rows * 8];
    tile_buff[0] = SSD_DATA;
    for (int r = 0; r < num_rows; r++) {
        for (int c = 0; c < num_cols; c++) {
            for (int i = 0; i < 8; i++) {
                tile_buff[1 + r * num_cols * 8 + c * 8 + i] =
                    DISP_BUFF[1 + (row + r) * SSD_SCREEN_WIDTH + ((col + c) << 3) + i];
            }
        }
    }

    // Set bounds
    const uint8_t set_limits[8] = {
        SSD_CTRL, SSD_CMD_SET_PAGE_ADDR, row, row + num_rows - 1,
        SSD_CTRL, SSD_CMD_SET_COL_ADDR, col << 3, (col << 3) + (num_cols << 3) - 1
    };
    if (write(I2C_FILE, set_limits, 8) != 8) {
        perror("SSD1306 - Warning. Failed to set sprite dispatch limits");
    }

    // Send data
    if (write(I2C_FILE, tile_buff, 1 + num_cols * num_rows * 8) != 1 + num_cols * num_rows * 8) {
        perror("SSD1306 - Warning. Failed to dispatch sprite data");
    }
}

void ssd1306__spr_dispatch(const uint8_t spr[8], uint8_t x, uint8_t y) {
    uint8_t tile_buff[1 + 4 * 8] = { 0 };
    tile_buff[0] = SSD_DATA;
    const uint8_t spr_col = x >> 3;
    const uint8_t spr_row = y >> 3;
    const uint8_t spr_x_off = x - (spr_col << 3);
    const uint8_t spr_y_off = y - (spr_row << 3);

    // Set top left tile
    for (int i = 0; i < 8; i++) {
        tile_buff[1 + i] = DISP_BUFF[1 + spr_row * SSD_SCREEN_WIDTH + (spr_col << 3) + i];
        if (i >= spr_x_off) {
            tile_buff[1 + i] |= spr[i - spr_x_off] << spr_y_off;
        }
    }

    // Set top right tile
    for (int i = 0; i < 8; i++) {
        tile_buff[1 + 8 + i] = DISP_BUFF[1 + spr_row * SSD_SCREEN_WIDTH + ((spr_col + 1) << 3) + i];
        if (i < spr_x_off) {
            tile_buff[1 + 8 + i] |= spr[7 - (spr_x_off - i - 1)] << spr_y_off;
        }
    }

    // Set bottom left tile
    for (int i = 0; i < 8; i++) {
        tile_buff[1 + 16 + i] =
            DISP_BUFF[1 + (spr_row + 1) * SSD_SCREEN_WIDTH + (spr_col << 3) + i];
        if (i >= spr_x_off) {
            tile_buff[1 + 16 + i] |= spr[i - spr_x_off] >> (8 - spr_y_off);
        }
    }

    // Set bottom right tile
    for (int i = 0; i < 8; i++) {
        tile_buff[1 + 24 + i] =
            DISP_BUFF[1 + (spr_row + 1) * SSD_SCREEN_WIDTH + ((spr_col + 1) << 3) + i];
        if (i < spr_x_off) {
            tile_buff[1 + 24 + i] |= spr[7 - (spr_x_off - i - 1)] >> (8 - spr_y_off);
        }
    }

    // Set bounds for sprite (a quad-tile)
    const uint8_t set_limits[8] = {
        SSD_CTRL, SSD_CMD_SET_PAGE_ADDR, spr_row, spr_row + 1,
        SSD_CTRL, SSD_CMD_SET_COL_ADDR, spr_col << 3, (spr_col << 3) + 15
    };
    if (write(I2C_FILE, set_limits, 8) != 8) {
        perror("SSD1306 - Warning. Failed to set sprite dispatch limits");
    }

    // Send data
    if (write(I2C_FILE, tile_buff, 1 + 4 * 8) != 1 + 4 * 8) {
        perror("SSD1306 - Warning. Failed to dispatch sprite data");
    }
}

void ssd1306__clear_spr_dispatch(uint8_t x, uint8_t y) {
    const uint8_t spr_col = x >> 3;
    const uint8_t spr_row = y >> 3;
    ssd1306__copy_and_partial_dispatch(spr_col, spr_row, 2, 2);
}

void ssd1306__vscroll(const uint8_t rows) {
    uint8_t buff[3] = {
        SSD_CTRL,
            SSD_CMD_SET_DISP_OFFSET, rows
    };
    if (write(I2C_FILE, buff, 3) != 3) {
        perror("SSD1306 - Warning. Failed to set offset");
    }
}

void ssd1306__deinit(void) {
    close(I2C_FILE);
}

void ssd1306__test(void) {
    // Fill 4 times as fast as possible
    for (int i = 0; i < 4; i++) {
        ssd1306__fill(false);
        ssd1306__dispatch();
        ssd1306__fill(true);
        ssd1306__dispatch();
    }

    for (int i = 0; i < 4; i++) {
        ssd1306__fill(false);
        ssd1306__copy_and_partial_dispatch(i + 3, i + 1, 4, 3);
        usleep(16);
        ssd1306__fill(true);
        ssd1306__copy_and_partial_dispatch(i + 3, i + 1, 4, 3);
    }
    ssd1306__fill(false);
    ssd1306__copy_and_partial_dispatch(7, 5, 4, 3);
    usleep(1000000);

    // Draw tile map
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 16; col++) {
            ssd1306__copy_tile(SPR_A_TEST, col, row);
        }
    }
    ssd1306__dispatch();
    usleep(1000);

    float scroll = 0.0;
    while (scroll < 8) {
        ssd1306__vscroll((uint8_t) scroll);
        scroll += 0.00167;
    }
    ssd1306__vscroll(0);
    usleep(1000);

    // Draw some sprites
    for (int row = 0; row < 8; row += 2) {
        for (int col = 0; col < 16; col += 2) {
            ssd1306__copy_tile(TILE_QUAD_TEST[0], col, row);
            ssd1306__copy_tile(TILE_QUAD_TEST[1], col + 1, row);
            ssd1306__copy_tile(TILE_QUAD_TEST[2], col, row + 1);
            ssd1306__copy_tile(TILE_QUAD_TEST[3], col + 1, row + 1);
            ssd1306__copy_and_partial_dispatch(col, row, 2, 2);
        }
    }
    ssd1306__copy_tile(SPR_A_TEST, 1, 1);
    ssd1306__copy_and_partial_dispatch(1, 1, 1, 1);
    float x = 39.0;
    uint8_t last_x = (uint8_t) x;
    uint8_t y = (uint8_t) x;
    ssd1306__spr_dispatch(SPR_A_TEST, (uint8_t) x, y);
    while (x > 0 && y > 0) {
        if ((uint8_t) x != last_x) {
            ssd1306__clear_spr_dispatch(last_x, y);
            y--;
            ssd1306__spr_dispatch(SPR_A_TEST, (uint8_t) x, y);
            last_x = x;
        }
        x -= 0.00167;
    }
    usleep(1000000);

    ssd1306__fill(true);
    ssd1306__dispatch();
}

// -------- Private Fns --------

// Open the sysfs to talk to the I2C port
bool open_i2c(void) {
    I2C_FILE = open(I2C_FNAME, O_RDWR);
    if (I2C_FILE < 0) {
        perror("SSD1306 - Failed to open I2C");
        return false;
    }
    if (ioctl(I2C_FILE, I2C_SLAVE, SSD_I2C_ADDR) < 0) {
        perror("SSD1306 - Failed to set I2C slave address");
        return false;
    }
    return true;
}

// Send a command to the SSD1306
static void i2c_cmd(uint8_t cmd) {
    const uint8_t buff[2] = { SSD_CTRL, cmd };
    if (write(I2C_FILE, buff, 2) != 2) {
        perror("SSD1306 - Warning. Failed to write command");
    }
}
