#include "ftoa.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MKL05Z4.h"

#include "cmdline.h"
#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include "mma8451q.h"
#include "pwm.h"
#include "spi.h"
#include "u8x8.h"
#include "u8x8_mkl_hal.h"
#include "uart.h"
#include "ws2812b.h"
#include "adc.h"

#include "config.h"
#include "microrl.h"

#define CMD_HELP "help"
#define CMD_CLEAR "clear"
#define CMD_I2C "i2c"
#define CMD_SPI "spi"
#define CMD_LED "led"
#define CMD_PWMTEST "pwmtest"
#define CMD_ADC "adc"

#define SUBCMD_MMA_TEST "mma_test"
#define SUBCMD_MMA_READALL "mma_read_all"
#define SUBCMD_I2C_BUSSCAN "busscan"
#define SUBCMD_SPI_TEST "display_test"
#define SUBCMD_LED_TEST "test"

#define SUBCMD_ADC_INIT "init"
#define SUBCMD_ADC_READ "read"
#define SUBCMD_ADC_KPAD_TEST "keypad_test"
#define SUBCMD_ADC_KB_TEST "keyboard_test"

#define NUM_OF_CMD 7
#define NUM_OF_I2C_SCMD 3
#define NUM_OF_SPI_SCMD 1
#define NUM_OF_LED_SCMD 1
#define NUM_OF_ADC_SCMD 4


// Internal microrl pointer
static microrl_t* prl = NULL;

// Char for adding new char to microrl to avoid running everything
// in interrupt context
static volatile bool new_char = false;
static volatile uint8_t new_char_data = 0;

// u8x8 struct
u8x8_t u8x8;

bool run_task = false;

static char string_buffer[10] = { 0 };
static uint8_t mma_init_done = 0;
static mma8451q mma;

//available  commands
char* keyworld[] = { CMD_HELP, CMD_CLEAR, CMD_I2C, CMD_SPI, CMD_LED, CMD_PWMTEST, CMD_ADC };
char* i2c_subcommands[] = { SUBCMD_MMA_TEST, SUBCMD_MMA_READALL, SUBCMD_I2C_BUSSCAN };
char* spi_subcommands[] = { SUBCMD_SPI_TEST };
char* led_subcommands[] = { SUBCMD_LED_TEST };
char* adc_subcommands[] = { SUBCMD_ADC_INIT, SUBCMD_ADC_READ, SUBCMD_ADC_KPAD_TEST, SUBCMD_ADC_KB_TEST };

// array for comletion
char* compl_world[NUM_OF_CMD + 1];

void print_help(void)
{
    uart_print("Use TAB key for completion\n\rCommand:\n\r");
    uart_print("\tclear               - clear screen\n\r");
    uart_print("\ti2c                 - I2C driver commands\n\r");
}

void i2c_busscan(void)
{
    uart_print("I2C bus scan results: \r\n");
    for (uint8_t addr = 1; addr <= 127; addr++) {
        i2c_status_t restult = i2c_ping(addr);
        if (restult == I2C_OK) {
            uart_print("Found device: ");
            sprintf(string_buffer, "%02x\r\n", addr);
            uart_print(string_buffer);
        }
    }
}

void mma_test(void)
{
    uart_print("Testing MMA8451Q accelerometer\r\n");
    mma8451_status_t mma_result = mma8451_init(&mma, MMA8451_DEFAULT_ADDRESS);
    if (mma_result)
        uart_print("MMA8451Q init failed\r\n");
    else {
        uart_print("MMA8451 init completed\r\n");
        mma_init_done = 1;
    }
}

void mma_readall(void)
{
    mma8451_status_t mma_result;
    float tmp = 0.0;
    if (!mma_init_done) {
        mma_result = mma8451_init(&mma, MMA8451_DEFAULT_ADDRESS);
        if (mma_result)
            uart_print("MMA8451Q init failed\r\n");
        mma_init_done = 1;
    }
    mma_result = mma8451_read(&mma);
    if (mma_result)
        uart_print("MMA8451Q read failed\r\n");
    uart_print("\r\nX: ");
    tmp = mma8451_get_x(&mma);
    ftoa(tmp, string_buffer, 2);
    uart_print(string_buffer);
    uart_print("\r\nY: ");
    tmp = mma8451_get_y(&mma);
    ftoa(tmp, string_buffer, 2);
    uart_print(string_buffer);
    uart_print("\r\nZ: ");
    tmp = mma8451_get_z(&mma);
    ftoa(tmp, string_buffer, 2);
    uart_print(string_buffer);
    uart_print("\r\n");
}

void spi_test(void)
{
    uart_print("Testing SPI Display\r\n");
    u8x8_Setup(&u8x8, u8x8_d_pcd8544_84x48, u8x8_cad_001, u8x8_byte_mkl05_hw_spi, u8x8_gpio_and_delay_mkl05);
    u8x8_InitDisplay(&u8x8);
    u8x8_SetPowerSave(&u8x8, 0);
    u8x8_ClearDisplay(&u8x8);
    u8x8_SetFont(&u8x8, u8x8_font_amstrad_cpc_extended_f);
    u8x8_DrawString(&u8x8, 0, 0, "Display");
    u8x8_DrawString(&u8x8, 0, 2, "Test");
}

void led_test(void)
{
    uart_print("Testing WS2811B LEDs \r\n");
    gpio_pin_t rgb_led = { PORTB, GPIOB, 5 };
    ws2812b_init(rgb_led);
    uint8_t data[] = { 0x00, 0xaa, 0xaa, 0xaa, 0x00, 0x55, 0x55, 0x55 };
    ws2812b_write(rgb_led, data, 8);
}

void pwm_test(void)
{
    uart_print("Testing PWM \r\n");
    gpio_pin_t green_led = { PORTB, GPIOB, 9 };
    pwm_pin_t green_led_pwm = pwm_from_gpio(green_led);
    if (pwm_init(green_led, green_led_pwm)) {
        uart_print("PWM config failed\r\n");
        return;
    }
    for (uint8_t i = 100; i > 0; i--) {
        pwm_pulsewidth_us(green_led_pwm, i * 200);
        delay_micro_seconds(10000);
    }
    for (uint8_t i = 0; i <= 100; i++) {
        pwm_pulsewidth_us(green_led_pwm, i * 200);
        delay_micro_seconds(10000);
    }
//     gpio_pin_t red_led = {PORTB, GPIOB, 8};
//     gpio_pin_init(red_led, GPIO_OUTPUT, GPIO_MUX_ALT1);
//     gpio_fast_off(red_led);
//     delay_micro_seconds(1000000);
//     gpio_fast_on(red_led);
//     delay_micro_seconds(1000000);
}

void adc_test_init(void) {
    gpio_pin_t adc_a0_pin = {PORTB, GPIOB, 13};
    adc_pin_t adc_a0 = adc_from_gpio(adc_a0_pin);
    adc_status_t cal_result = adc_init(adc_a0_pin, adc_a0, ADC_CONFIG_LOWPOWER_FAST);
    if (cal_result)
        uart_print("ADC init failed\r\n");
    else {
        uart_print("ADC init completed\r\n");
    }
}

void adc_test_read(void) {
    gpio_pin_t adc_a0_pin = {PORTB, GPIOB, 13};
    adc_pin_t adc_a0 = adc_from_gpio(adc_a0_pin);
    uint16_t val = adc_read(adc_a0);
    uart_print("\r\n ADC Read: ");
    sprintf(string_buffer, "%d\r\n", val);
    uart_print(string_buffer);
}

void adc_print_on_display(gpio_pin_t adc_pin) {
    uart_print("\r\n Starting display loop, press Ctrl-C to stop \r\n");
    adc_pin_t adc_channel = adc_from_gpio(adc_pin);
    adc_init(adc_pin, adc_channel, ADC_CONFIG_LOWPOWER_FAST);
    u8x8_Setup(&u8x8, u8x8_d_pcd8544_84x48, u8x8_cad_001, u8x8_byte_mkl05_hw_spi, u8x8_gpio_and_delay_mkl05);
    u8x8_InitDisplay(&u8x8);
    u8x8_SetPowerSave(&u8x8, 0);
    u8x8_ClearDisplay(&u8x8);
    u8x8_SetFont(&u8x8, u8x8_font_amstrad_cpc_extended_f);
    run_task = true;
    uint16_t adc_val = 0;
    while (run_task)
    {
        u8x8_ClearDisplay(&u8x8);
        u8x8_DrawString(&u8x8, 0, 0, "ADC Value:");
        adc_val = adc_read(adc_channel);
        sprintf(string_buffer, "%d", adc_val);
        u8x8_DrawString(&u8x8, 0, 2, string_buffer);
        for (uint32_t i = 0; i < (100 * 10000); i++)
            __NOP();
    }
}



// execute callback for microrl library
int execute(int argc, const char* const* argv)
{
    int i = 0;
    while (i < argc) {
        if (strcmp(argv[i], CMD_HELP) == 0) {
            uart_print("KL05-Devboard shell, using microrl v");
            uart_print(MICRORL_LIB_VER);
            uart_print("\n\r");
            print_help(); // uart_printhelp
        } else if (strcmp(argv[i], CMD_CLEAR) == 0) {
            uart_print("\033[2J"); // ESC seq for clear entire screen
            uart_print("\033[H"); // ESC seq for move cursor at left-top corner
        } else if (strcmp(argv[i], CMD_PWMTEST) == 0) {
            pwm_test();
        } else if (strcmp(argv[i], CMD_I2C) == 0) {
            if (++i < argc) {
                if (strcmp(argv[i], SUBCMD_MMA_TEST) == 0) {
                    mma_test();
                } else if (strcmp(argv[i], SUBCMD_MMA_READALL) == 0) {
                    mma_readall();
                } else if (strcmp(argv[i], SUBCMD_I2C_BUSSCAN) == 0) {
                    i2c_busscan();
                } else {
                    uart_print("I2C subcommand not found\n\r");
                }
            } else {
                uart_print("Too little arguments\n\r");
            }
        } else if (strcmp(argv[i], CMD_SPI) == 0) {
            if (++i < argc) {
                if (strcmp(argv[i], SUBCMD_SPI_TEST) == 0) {
                    spi_test();
                } else {
                    uart_print("SPI subcommand not found\n\r");
                }
            } else {
                uart_print("Too little arguments\n\r");
            }
        } else if (strcmp(argv[i], CMD_ADC) == 0) {
            if (++i < argc) {
                if (strcmp(argv[i], SUBCMD_ADC_INIT) == 0) {
                    adc_test_init();
                } else if (strcmp(argv[i], SUBCMD_ADC_READ) == 0) {
                    adc_test_read();
                } else if (strcmp(argv[i], SUBCMD_ADC_KPAD_TEST) == 0) {
                    gpio_pin_t kpad_gpio = {PORTA, GPIOA, 8};
                    adc_print_on_display(kpad_gpio);
                } else if (strcmp(argv[i], SUBCMD_ADC_KB_TEST) == 0) {
                    gpio_pin_t kboard_gpio = {PORTA, GPIOA, 9};
                    adc_print_on_display(kboard_gpio);
                } else {
                    uart_print("ADC subcommand not found\n\r");
                }
            } else {
                uart_print("Too little arguments\n\r");
            }
        } else if (strcmp(argv[i], CMD_LED) == 0) {
            if (++i < argc) {
                if (strcmp(argv[i], SUBCMD_LED_TEST) == 0) {
                    led_test();
                } else {
                    uart_print("LED subcommand not found\n\r");
                }
            } else {
                uart_print("Too little arguments\n\r");
            }
        } else {
            uart_print("command: '");
            uart_print((char*)argv[i]);
            uart_print("' Not found.\n\r");
        }
        i++;
    }
    return 0;
}

#ifdef _USE_COMPLETE
//*****************************************************************************
// completion callback for microrl library
char** complet(int argc, const char* const* argv)
{
    int j = 0;

    compl_world[0] = NULL;

    // if there is token in cmdline
    if (argc == 1) {
        // get last entered token
        char* bit = (char*)argv[argc - 1];
        // iterate through our available token and match it
        for (int i = 0; i < NUM_OF_CMD; i++) {
            // if token is matched (text is part of our token starting from 0 char)
            if (strstr(keyworld[i], bit) == keyworld[i]) {
                // add it to completion set
                compl_world[j++] = keyworld[i];
            }
        }
    } else if ((argc > 1) && ((strcmp(argv[0], CMD_I2C) == 0))) { // if command needs subcommands
        // iterate through subcommand
        for (int i = 0; i < NUM_OF_I2C_SCMD; i++) {
            if (strstr(i2c_subcommands[i], argv[argc - 1]) == i2c_subcommands[i]) {
                compl_world[j++] = i2c_subcommands[i];
            }
        }
    } else if ((argc > 1) && ((strcmp(argv[0], CMD_SPI) == 0))) {
        // iterate through subcommand
        for (int i = 0; i < NUM_OF_SPI_SCMD; i++) {
            if (strstr(spi_subcommands[i], argv[argc - 1]) == spi_subcommands[i]) {
                compl_world[j++] = spi_subcommands[i];
            }
        }
    } else if ((argc > 1) && ((strcmp(argv[0], CMD_LED) == 0))) {
        // iterate through subcommand
        for (int i = 0; i < NUM_OF_LED_SCMD; i++) {
            if (strstr(led_subcommands[i], argv[argc - 1]) == led_subcommands[i]) {
                compl_world[j++] = led_subcommands[i];
            }
        }
    } else if ((argc > 1) && ((strcmp(argv[0], CMD_ADC) == 0))) {
        // iterate through subcommand
        for (int i = 0; i < NUM_OF_ADC_SCMD; i++) {
            if (strstr(adc_subcommands[i], argv[argc - 1]) == adc_subcommands[i]) {
                compl_world[j++] = adc_subcommands[i];
            }
        }
    } else { // if there is no token in cmdline, just print all available token
        for (; j < NUM_OF_CMD; j++) {
            compl_world[j] = keyworld[j];
        }
    }

    // note! last ptr in array always must be NULL!!!
    compl_world[j] = NULL;
    // return set of variants
    return compl_world;
}
#endif

void sigint(void)
{
    uart_print("^C detected, stopping task\n\r");
    run_task = false;
}

void rx_interrupt()
{
    if (uart_available()) {
        rx_overrun(); // TODO: Add FIFO to avoid this mess
        new_char_data = uart_getchar();
        if (new_char_data == KEY_ETX) {
            sigint();
        } else {
            new_char = true;
        }
    }
}

cmdline_status_t cmdline_init(microrl_t* readline)
{
    prl = readline;
    microrl_init(prl, uart_print);
    microrl_set_execute_callback(prl, execute);
#ifdef _USE_COMPLETE
    // set callback for completion
    microrl_set_complete_callback(prl, complet);
#endif
    microrl_set_sigint_callback(prl, sigint);

    uart_set_irq(rx_interrupt);
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);
    return CMDLINE_OK;
}

void process_uart() {
    if (new_char) {
        new_char = false;
        if (prl != NULL)
        {
            microrl_insert_char(prl, new_char_data);
        }
    }
}