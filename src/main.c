#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "string.h"

int I2C_SDA = 18;
int I2C_SCL = 19;

const int SPI_DISP_SCK = 34;
const int SPI_DISP_CSn = 33; 
const int SPI_DISP_TX = 35;

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
    init_chardisp_pins();
    cd_init();
    
  //  eeprom_write(0x20, &clear_val, 1);

    eeprom_read(0x20, &high_score, 1);
    if (round_score > high_score) {
        eeprom_write(0x20, &round_score, 1);
        eeprom_read(0x20, &high_score, 1);
    }
    // printf("High Score: %u\n", high_score);
    cd_display1("High Score: ");
    cd_display2(high_score);
    
}

void init_chardisp_pins() {
    gpio_set_function(SPI_DISP_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_DISP_CSn, GPIO_FUNC_SPI);        
    gpio_set_function(SPI_DISP_TX, GPIO_FUNC_SPI);
 
    spi_init(spi0, 10000);
    spi_set_format(spi0, 9, 0, 0, 1);
    spi_set_slave(spi0, 0);
}

void send_spi_cmd(spi_inst_t* spi, uint16_t value) {
    bool is_busy = spi_is_busy(spi0);
    while(is_busy) {}
    spi_write16_blocking(spi, &value, 1);
}

void send_spi_data(spi_inst_t* spi, uint16_t value) {
    send_spi_cmd(spi0, (value | 0x100));
}

void cd_init() {
    sleep_ms(1);
    
    send_spi_cmd(spi0, 0x38);
    sleep_us(40);
    
    send_spi_cmd(spi0, 0x0C);
    sleep_us(40);

    send_spi_cmd(spi0, 0x01);
    sleep_ms(2);

    send_spi_cmd(spi0, 0x06);
    sleep_us(40);
}

void cd_display1(const char *str) {
    send_spi_cmd(spi0, 0x80); 
    for (int i = 0; i < 16; i++) {
        send_spi_data(spi0, (uint16_t)str[i]);
    }
}
void cd_display2(uint8_t value) {
    send_spi_cmd(spi0, 0x80 | 0x40);
    send_spi_data(spi0, (uint16_t)value);
}