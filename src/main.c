#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "string.h"
#include <stdio.h>

//adc stuff for joystick
#define CENTER_TOLERANCE 500  // range around midpoint considered neutral
#define ADC_MIDPOINT 2048 // ~1.65V for 12-bit ADC (0–4095)
#define ADC_MAX 4095 //max value for 12 bit adc

#define JOY_X 0  // gp26 connect to vrX
#define JOY_Y 1  // gp27 connect to vrY

typedef enum {
    DIR_CENTER,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} direction_t;

//i2c stuff
int I2C_SDA = 18;
int I2C_SCL = 19;

uint8_t clear_val = 0;
uint8_t high_score;
uint8_t round_score = 13;

void adc_init_joystick() {
    adc_init();
    adc_gpio_init(26);  // ADC0 (X-axis)
    adc_gpio_init(27);  // ADC1 (Y-axis)
}

// Read raw X and Y values
void read_joystick_raw(uint16_t *x, uint16_t *y) {
    adc_select_input(JOY_X);
    *x = adc_read();
    adc_select_input(JOY_Y);
    *y = adc_read();
}

// Convert raw ADC readings into a direction
direction_t get_joystick_direction(void) {
    uint16_t x, y;
    read_joystick_raw(&x, &y);

    int dx = (int)x - ADC_MIDPOINT;
    int dy = (int)y - ADC_MIDPOINT;

    if (abs(dx) < CENTER_TOLERANCE && abs(dy) < CENTER_TOLERANCE) {
        return DIR_CENTER;
    }

    if (abs(dy) > abs(dx)) {
        //y direction dominates
        if (dy > CENTER_TOLERANCE) return DIR_UP;
        else if (dy < -CENTER_TOLERANCE) return DIR_DOWN;
    } else {
        //x direction dominates
        if (dx > CENTER_TOLERANCE) return DIR_RIGHT;
        else if (dx < -CENTER_TOLERANCE) return DIR_LEFT;
    }

    return DIR_CENTER; // fallback
}

void init_i2c() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
}

void eeprom_write(uint16_t loc, const void *data, size_t len) {
    uint8_t tx[2 + len];
    tx[0] = (uint8_t)(loc >> 8);
    tx[1] = (uint8_t)loc;
    memcpy(&tx[2], data, len);
    i2c_write_blocking(i2c1, 0x50, tx, 2 + len, false); 
    sleep_ms(5); 
}

void eeprom_read(uint16_t loc, void *data, size_t len) {
    uint8_t a[2] = { (uint8_t)(loc >> 8), (uint8_t)loc };
    i2c_write_blocking(i2c1, 0x50, a, 2, true);
    i2c_read_blocking(i2c1, 0x50, (uint8_t*)data, len, false);
}

int main() {
    stdio_init_all();
    adc_init_joystick();
    init_i2c();
  //  eeprom_write(0x20, &clear_val, 1);

    while (true) {
        direction_t dir = get_joystick_direction();

        switch (dir) {
            case DIR_UP:
                printf("UP (Red)\n");
                break;
            case DIR_DOWN:
                printf("DOWN (Blue)\n");
                break;
            case DIR_LEFT:
                printf("LEFT (Yellow)\n");
                break;
            case DIR_RIGHT:
                printf("RIGHT (Green)\n");
                break;
            default:
                break;
        }

        sleep_ms(200); // Debounce / slow down prints
    }

    eeprom_read(0x20, &high_score, 1);
    if (round_score > high_score) {
        eeprom_write(0x20, &round_score, 1);
        eeprom_read(0x20, &high_score, 1);
    }
    printf("High Score: %u\n", high_score);
}