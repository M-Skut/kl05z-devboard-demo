#ifndef I2C_H_INCLUDE
#define I2C_H_INCLUDE

#include <stdint.h>

typedef enum i2c_status_e {
    I2C_OK = 0,
    I2C_OK_NACK = 1,
    I2C_CONFIGERR_GPIO = 2,
    I2C_ERR_TIMEOUT = 3,
} i2c_status_t;

typedef enum i2c_scl_pin_e {
    I2C_PTA3_SCL = 1,
    I2C_PTA4_SCL = 2,
    I2C_PTB3_SCL = 3,
} i2c_scl_pin_t;

typedef enum i2c_sda_pin_e {
    I2C_PTA3_SDA = 4,
    I2C_PTA4_SDA = 5,
    I2C_PTB4_SDA = 6,
} i2c_sda_pin_t;


i2c_status_t i2c_init(i2c_scl_pin_t scl, i2c_sda_pin_t sda, uint32_t speed);

i2c_status_t i2c_start(void);

i2c_status_t i2c_stop(void);

i2c_status_t i2c_read(uint8_t address, uint8_t *register_list, uint8_t list_length, uint8_t stop);

i2c_status_t i2c_write(uint8_t address, uint8_t *register_list, uint8_t list_length, uint8_t stop);

i2c_status_t i2c_ping(uint8_t address);

i2c_status_t i2c_read_byte(uint8_t address, uint8_t *device_register, uint8_t last);

i2c_status_t i2c_write_byte(uint8_t data);

#endif
