#ifndef SPI_H_INCLUDE
#define SPI_H_INCLUDE

#include <stdint.h>


typedef enum spi_status_e {
    SPI_OK = 0,
    SPI_CONFIGERR_PERIPH = 1,
} spi_status_t;

typedef enum spi_mode_e {
    SPI_MODE_0 = 0,
    SPI_MODE_1 = 1,
    SPI_MODE_2 = 2,
    SPI_MODE_3 = 3,
} spi_mode_t;

typedef enum spi_side_e {
    SPI_SIDE_SLAVE = 0,
    SPI_SIDE_MASTER = 1,
} spi_side_t;

/**
 * @brief Initialize SPI peripheral
 * This driver only supports mode and speed setting. On MCU used 
 * in KL05 devboard there is only one possible combination on SPI outputs.
 * @param mode spi_mode_t mode, for more refer to SPI protocol documentation
 * @param speed desired speed in hz. If not possible to set the exact speed, the driver will
 * set the closest one. 
 * @param side spi_side_t SPI communication type. If set to SPI_SIDE_MASTER, the MCU will be the
 * master, for SPI_SIDE_SLAVE the MCU will be configured as SPI slave. In slave mode, pin PTA5 will
 * be used as SS pin.
 * Hardcoded pin configuration:
 * MISO on PTA6
 * MOSI on PTA7
 * SCK on PTB0
 * SS on PTA5 for slave mode
 * @return spi_status_t Driver status output. If function was called with correct parameters it
 * will return SPI_OK status, otherwise SPI_CONFIGERR_PERIPH will be returned. 
 */
spi_status_t spi_init(spi_mode_t mode, uint32_t speed, spi_side_t side);
/**
 * @brief Perform SPI transaction
 * 
 * @param data 8 bit data to send to the device
 * @return uint8_t data read from the device in the transaction
 */
uint8_t spi_master_write(uint8_t data);

/**
 * @brief Write data to the master
 * TODO: check for possible timeout
 * @param data 8-bit data to be send for the device
 */
void spi_slave_write(uint8_t data);

/**
 * @brief Check for new data in SPI buffer.
 * 
 * 
 * @return uint8_t non-zero if new data was sent from the master. 
 */
uint8_t spi_slave_receive(void);

/**
 * @brief Read data from SPI buffer.
 * Suggested to use `spi_slave_receive` to check for
 * new data and then read using this function.
 * 
 * @return uint8_t data from SPI buffer.
 */
uint8_t spi_slave_read(void);

#endif
