#ifndef MMA8451Q_H_INCLUDE
#define MMA8451Q_H_INCLUDE

#include <stdint.h>
#include <stdlib.h>

#define MMA8451_DEFAULT_ADDRESS 0x1D
#define MMA8451_REG_OUT_X_MSB 0x01
#define MMA8451_REG_SYSMOD 0x0B
#define MMA8451_REG_WHOAMI 0x0D
#define MMA8451_REG_WHOAMI_VAL 0x1A
#define MMA8451_REG_XYZ_DATA_CFG 0x0E
#define MMA8451_REG_PL_STATUS 0x10
#define MMA8451_REG_PL_CFG 0x11
#define MMA8451_REG_CTRL_REG1 0x2A
#define MMA8451_REG_CTRL_REG2 0x2B
#define MMA8451_REG_CTRL_REG4 0x2D
#define MMA8451_REG_CTRL_REG5 0x2E

#define MMA8451_REG_CTRL_REG1_ACTIVE (1 << 0)


#define MMA8451_REG_CTRL_REG2_RST (1 << 6)
#define MMA8451_REG_CTRL_REG2_MODS1 (1 << 1)

#define MMA8451_RANGE_MASK 0x03

typedef enum mma8451_range_e {
    MMA8451_RANGE_8_G = 0x02,
    MMA8451_RANGE_4_G = 0x01,
    MMA8451_RANGE_2_G = 0x00,
} mma8451_range_t;

typedef enum mma8451_datarate_e {
    MMA8451_DATARATE_800_HZ = 0x00,
    MMA8451_DATARATE_400_HZ = 0x01,
    MMA8451_DATARATE_200_HZ = 0x02,
    MMA8451_DATARATE_100_HZ = 0x03,
    MMA8451_DATARATE_50_HZ = 0x04,
    MMA8451_DATARATE_12_5_HZ = 0x05,
    MMA8451_DATARATE_6_25HZ = 0x06,
    MMA8451_DATARATE_1_56_HZ = 0x07,
} mma8451_datarate_t;

typedef enum mma8451_status_e {
    MMA8451_OK = 0,
    MMA8451_ERROR_NODEVICE = 1,
    MMA8451_ERROR_TIMEOUT = 2,
    MMA8451_ERROR_RST_FAILED = 3,
    MMA8451_ERROR_CONFIG_FAILED = 4,
} mma8451_status_t;

typedef struct mma8451q_s {
    uint8_t address;
    uint16_t range_divider;
    int16_t x, y, z;
} mma8451q;

mma8451_status_t mma8451_init(mma8451q *device, uint8_t address);
mma8451_status_t mma8451_read(mma8451q *device);
mma8451_status_t mma8451_set_range(mma8451q *device, mma8451_range_t sensor_range);
float mma8451_get_x(mma8451q *device);
float mma8451_get_y(mma8451q *device);
float mma8451_get_z(mma8451q *device);
#endif
