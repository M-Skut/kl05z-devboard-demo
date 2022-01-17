#include "mma8451q.h"
#include "MKL05Z4.h"
#include "i2c.h"
#include <stdbool.h>
#include <string.h>

static i2c_status_t mma8451_readreg(uint8_t address, uint8_t reg, uint8_t* data)
{
    i2c_status_t i2c_result = i2c_write(address, &reg, 1, false);
    if (i2c_result)
        return i2c_result;
    i2c_result = i2c_read(address, data, 1, true);
    return i2c_result;
}

static i2c_status_t mma8451_writereg(uint8_t address, uint8_t reg, uint8_t val)
{
    uint8_t buffer[2] = { reg, val };
    return i2c_write(address, buffer, 2, true);
}

static i2c_status_t mma8451_set_status(uint8_t address, uint8_t active)
{
    uint8_t reg1;
    i2c_status_t i2c_result = mma8451_readreg(address, MMA8451_REG_CTRL_REG1, &reg1);
    if (i2c_result)
        return i2c_result;
    if (active) {
        i2c_result = mma8451_writereg(address, MMA8451_REG_CTRL_REG1, (reg1 | MMA8451_REG_CTRL_REG1_ACTIVE));
    } else {
        i2c_result = mma8451_writereg(address, MMA8451_REG_CTRL_REG1, (reg1 & ~MMA8451_REG_CTRL_REG1_ACTIVE));
    }
    return i2c_result;
}

mma8451_status_t mma8451_init(mma8451q* device, uint8_t address)
{
    uint8_t whoami = 0x00;
    uint8_t tmp = 0xFF;
    i2c_status_t i2c_result = I2C_OK;
    memset(device, 0, sizeof(mma8451q));
    device->address = address;
    device->range_divider = 4096; // For default 2G sensor range

    // Check if the device is present
    mma8451_readreg(device->address, MMA8451_REG_WHOAMI, &whoami);
    if (whoami != MMA8451_REG_WHOAMI_VAL)
        return MMA8451_ERROR_NODEVICE;

    // Run soft reset
    mma8451_writereg(device->address, MMA8451_REG_CTRL_REG2, MMA8451_REG_CTRL_REG2_RST);
    uint32_t timeout = 0;
    while ((tmp & MMA8451_REG_CTRL_REG2_RST) || timeout > 100) {
        i2c_result = mma8451_readreg(device->address, MMA8451_REG_CTRL_REG2, &tmp);
        if (i2c_result == I2C_ERR_TIMEOUT)
            return MMA8451_ERROR_TIMEOUT;
        timeout++;
    }
    if (timeout >= 1000)
        return MMA8451_ERROR_RST_FAILED;

    // Set high resolution mode
    i2c_result = mma8451_writereg(device->address, MMA8451_REG_CTRL_REG2, MMA8451_REG_CTRL_REG2_MODS1);
    if (i2c_result)
        return MMA8451_ERROR_CONFIG_FAILED;
    // Enable accelerometer
    i2c_result = mma8451_writereg(device->address, MMA8451_REG_CTRL_REG1, MMA8451_REG_CTRL_REG1_ACTIVE);
    if (i2c_result)
        return MMA8451_ERROR_CONFIG_FAILED;

    return MMA8451_OK;
}

mma8451_status_t mma8451_read(mma8451q* device)
{
    uint8_t buffer[6] = { MMA8451_REG_OUT_X_MSB, 0, 0, 0, 0, 0 };
    i2c_status_t result = I2C_OK;
    result = i2c_write(device->address, buffer, 1, false);
    if (result)
        return MMA8451_ERROR_TIMEOUT;
    result = i2c_read(device->address, buffer, 6, true);
    if (result)
        return MMA8451_ERROR_TIMEOUT;

    device->x = buffer[0];
    device->x <<= 8;
    device->x |= buffer[1];
    device->x >>= 2;
    device->y = buffer[2];
    device->y <<= 8;
    device->y |= buffer[3];
    device->y >>= 2;
    device->z = buffer[4];
    device->z <<= 8;
    device->z |= buffer[5];
    device->z >>= 2;
    return MMA8451_OK;
}

mma8451_status_t mma8451_set_range(mma8451q* device, mma8451_range_t sensor_range)
{
    uint8_t reg1;
    mma8451_set_status(device->address, false);
    mma8451_readreg(device->address, MMA8451_REG_CTRL_REG1, &reg1);
    mma8451_writereg(device->address, MMA8451_REG_CTRL_REG1, (reg1 & ~MMA8451_RANGE_MASK) | sensor_range);
    mma8451_set_status(device->address, true);
    if (sensor_range == MMA8451_RANGE_2_G)
        device->range_divider = 4096;
    else if (sensor_range == MMA8451_RANGE_4_G)
        device->range_divider = 2048;
    else
        device->range_divider = 1024;

    return MMA8451_OK;
}

float mma8451_get_x(mma8451q* device)
{
    return (float)device->x / device->range_divider;
}

float mma8451_get_y(mma8451q* device)
{
    return (float)device->y / device->range_divider;
}

float mma8451_get_z(mma8451q* device)
{
    return (float)device->z / device->range_divider;
}