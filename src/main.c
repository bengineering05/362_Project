#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "string.h"

int I2C_SDA = 18;
int I2C_SCL = 19;

uint8_t clear_val = 0;
uint8_t high_score;
uint8_t round_score = 13;


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

    init_i2c();
  //  eeprom_write(0x20, &clear_val, 1);

    eeprom_read(0x20, &high_score, 1);
    if (round_score > high_score) {
        eeprom_write(0x20, &round_score, 1);
        eeprom_read(0x20, &high_score, 1);
    }
    printf("High Score: %u\n", high_score);
}