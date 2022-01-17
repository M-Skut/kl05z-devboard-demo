#include "i2c.h"
#include "MKL05Z4.h"
#include <stdint.h>

static const uint32_t max_timeout = 1000;

// Source: 36.4.1.10 I2C divider and hold values
static const uint16_t ICR[0x40] = {
    20, 22, 24, 26, 28, 30, 34, 40,
    28, 32, 36, 40, 44, 48, 56, 68,
    48, 56, 64, 72, 80, 88, 104, 128,
    80, 96, 112, 128, 144, 160, 192, 240,
    160, 192, 224, 256, 288, 320, 384, 480,
    320, 384, 448, 512, 576, 640, 768, 960,
    640, 768, 896, 1024, 1152, 1280, 1536, 1920,
    1280, 1536, 1792, 2048, 2304, 2560, 3072, 3840
};

static i2c_status_t i2c_wait_for_flag(uint8_t mask)
{
    for (uint32_t timeout = 0; timeout < max_timeout; timeout++) {
        if (I2C0->S & mask)
            return I2C_OK;
    }
    return I2C_ERR_TIMEOUT;
}

i2c_status_t i2c_init(i2c_scl_pin_t scl, i2c_sda_pin_t sda, uint32_t speed)
{
    i2c_status_t result = I2C_OK;
    uint8_t icr = 0;
    uint8_t mult = 0;
    uint8_t calc_mult = 1;
    uint32_t diff = 0;
    uint32_t speed_diff = UINT32_MAX;
    uint32_t calculated_speed = 0;
    uint32_t i2c_clock = SystemCoreClock / 2;

    // Enable clock for I2C
    SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;

    switch (scl) {
    case I2C_PTA3_SCL:
        SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
        PORTA->PCR[3] = PORT_PCR_MUX(2);
        break;
    case I2C_PTA4_SCL:
        SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
        PORTA->PCR[4] = PORT_PCR_MUX(3);
        break;
    case I2C_PTB3_SCL:
        SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
        PORTB->PCR[3] = PORT_PCR_MUX(2);
        break;
    default:
        result = I2C_CONFIGERR_GPIO;
        goto fail;
    }

    switch (sda) {
    case I2C_PTA3_SDA:
        SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
        PORTA->PCR[3] = PORT_PCR_MUX(3);
        break;
    case I2C_PTA4_SDA:
        SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
        PORTA->PCR[4] = PORT_PCR_MUX(2);
        break;
    case I2C_PTB4_SDA:
        SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
        PORTB->PCR[4] = PORT_PCR_MUX(2);
        break;
    default:
        result = I2C_CONFIGERR_GPIO;
        goto fail;
    }

    // Configure speed by looking for values that minimize the error
    // by testing all possible MULT and ICR combinations
    for (uint8_t m = 0; m < 2; m++) {
        for (uint8_t i = 0; i < 0x40; i++) {
            calculated_speed = i2c_clock / (calc_mult * ICR[i]);
            diff = (calculated_speed > speed) ? calculated_speed - speed : speed - calculated_speed;
            if (diff < speed_diff) {
                icr = i;
                mult = m;
                speed_diff = diff;
            }
        }
        // Test next multiplier
        calc_mult *= 2;
    }
    // Set calculated speed and enable I2C
    I2C0->F = icr | (mult << 6);
    I2C0->C1 = I2C_C1_IICEN_MASK;
    return result;

fail:
    // Disable I2C
    SIM->SCGC4 &= ~SIM_SCGC4_I2C0_MASK;
    return result;
}

i2c_status_t i2c_start(void)
{
    if (I2C0->S & I2C_S_BUSY_MASK) {
        // F register needs to be saved, set to zero and restored
        // source: e6070 errata for Mask 2N96F
        uint8_t tempF = I2C0->F;
        I2C0->F = 0;
        I2C0->C1 |= I2C_C1_RSTA_MASK | I2C_C1_TX_MASK;
        I2C0->F = tempF;
        return I2C_OK;
    } else {
        I2C0->C1 |= I2C_C1_MST_MASK;
        I2C0->C1 |= I2C_C1_TX_MASK;
        return i2c_wait_for_flag(I2C_S_BUSY_MASK);
    }
}

i2c_status_t i2c_stop(void)
{
    I2C0->C1 &= ~I2C_C1_MST_MASK;
    I2C0->C1 &= ~I2C_C1_TX_MASK;
    // Wait until STOP bit will be transferred
    for (uint32_t timeout = 0; timeout < max_timeout; timeout++) {
        if (!(I2C0->S & I2C_S_BUSY_MASK))
            return I2C_OK;
    }
    return I2C_ERR_TIMEOUT;
}

void i2c_send_nack(void)
{
    I2C0->C1 |= I2C_C1_TXAK_MASK;
}

void i2c_send_ack(void)
{
    I2C0->C1 &= ~I2C_C1_TXAK_MASK;
}

i2c_status_t i2c_write_frame(uint8_t value)
{
    I2C0->D = value;
    if (i2c_wait_for_flag(I2C_S_IICIF_MASK))
        return I2C_ERR_TIMEOUT;

    I2C0->S |= I2C_S_IICIF_MASK;

    if (i2c_wait_for_flag(I2C_S_TCF_MASK))
        return I2C_ERR_TIMEOUT;

    return (I2C0->S & I2C_S_RXAK_MASK) ? I2C_OK_NACK : I2C_OK;
}

i2c_status_t i2c_read_frame(uint8_t* data, uint8_t last)
{
    if (last)
        i2c_send_nack();
    else
        i2c_send_ack();

    *data = I2C0->D;

    if (i2c_wait_for_flag(I2C_S_IICIF_MASK))
        return I2C_ERR_TIMEOUT;

    I2C0->S |= I2C_S_IICIF_MASK;

    return I2C_OK;
}

i2c_status_t i2c_read(uint8_t address, uint8_t* register_list, uint8_t list_length, uint8_t stop)
{
    uint8_t dummy_read, count;
    i2c_status_t result = I2C_OK;
    uint8_t* current_reg;

    result = i2c_start();
    if (result)
        goto fail;

    result = i2c_write_frame((address << 1) | 0x01);
    if (result) {
        if (result == I2C_ERR_TIMEOUT)
            goto fail_stop;
        else
            goto fail;
    }
    // Go to RX mode to read data
    I2C0->C1 &= ~I2C_C1_TX_MASK;

    for (count = 0; count < list_length; count++) {
        current_reg = (count == 0) ? &dummy_read : &register_list[count - 1];
        uint8_t last = (count == (list_length - 1)) ? 1 : 0;
        result = i2c_read_frame(current_reg, last);
        if (result)
            goto fail;
    }

    // Send stop if requested and read last register
    if (stop)
        i2c_stop();

    register_list[count - 1] = I2C0->D;
    return result;

fail_stop:
    i2c_stop();
fail:
    return result;
}

i2c_status_t i2c_write(uint8_t address, uint8_t* register_list, uint8_t list_length, uint8_t stop)
{
    i2c_status_t result = I2C_OK;

    result = i2c_start();
    if (result)
        goto fail;

    result = i2c_write_frame((address << 1) & 0xFE);
    if (result) {
        if (result == I2C_ERR_TIMEOUT)
            goto fail_stop;
        else
            goto fail;
    }
    // Burst write data
    for (uint8_t count = 0; count < list_length; count++) {
        result = i2c_write_frame(register_list[count]);
        if (result) {
            if (result == I2C_ERR_TIMEOUT)
                goto fail_stop;
            else
                goto fail;
        }
    }
    // Send stop if requested
    if (stop)
        i2c_stop();

    return result;

fail_stop:
    i2c_stop();
fail:
    return result;
}

i2c_status_t i2c_ping(uint8_t address)
{
    i2c_status_t result = I2C_OK;
    result = i2c_start();
    if (result)
        goto func_end;
    result = i2c_write_frame(address << 1);

func_end:
    i2c_stop();
    return result;
}

i2c_status_t i2c_read_byte(uint8_t address, uint8_t* device_register, uint8_t last)
{
    uint8_t data;
    i2c_status_t result = I2C_OK;
    // set rx mode
    I2C0->C1 &= ~I2C_C1_TX_MASK;
    result = i2c_read_frame(&data, last);
    if (result)
        return I2C_ERR_TIMEOUT;
    // set tx mode
    I2C0->C1 |= I2C_C1_TX_MASK;
    *device_register = I2C0->D;

    return result;
}

i2c_status_t i2c_write_byte(uint8_t data)
{
    // set TX mode
    I2C0->C1 |= I2C_C1_TX_MASK;

    return i2c_write_frame(data);
}