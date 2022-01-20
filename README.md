# KL-05 DevBoard Demo project with simple HAL

## Hardware
To make development of advanced microcontroller projects simple, the DevBoard is equipped with following features:
- SD Card slot
- 6-button keypad connected as analog input
- stylophone-like keyboard as analog input
- PMOD Interface Type 2 (SPI)
- MikroBus Connector
- 3x WS2812B addressable RGB LEDs
- connector for Nokia 5510 display board
- buzzer
- fused 3V3 and 5V lines
- connector for Adafruit ALS-PT19 Analog Light Sensor Breakout
- connector for 16x2 LCD with I2C adapter
- 25LCxxx SPI Flash
- 2x 24Cxx I2C Flash
- RTC clock with battery based on PCF8563 RTC IC with I2C interface
- LM75 I2C temperature sensor

## Some photos of the hardware

## Bill Of Materials
Interactive Bill Of Materials for the newest revision of the hardware can be found [here](frdm05-devboard-hardware/bom/ibom.html)


#### Hardware revision history
Rev. 1:
- first revision of hardware
Rev. 2:
- fix for analog keypad (wrong placement of current mirror)
- addition of current-limiting resistor for buzzer
- addition of optional fuses for 3V3 and 5V lines
Rev. 2.1:
- fix for 3V3 fuse
- fix for buzzer footprint
- fix for some silkscreen areas
Rev. 2.2:
- change 74HCT1G125GV buffer to faster 74AHCT1G125GV version


## Environment
To develop this demonstration project, following set of tools and programs was used:
- [Visual Studio Code](https://code.visualstudio.com/) with plugins:
    - [`Cortex Debug`](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) from `marus25`
    - [`CMake Tools`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) from `microsoft`
- GNU GCC ARM Compiler from Official ARM Distribution
- SEGGER JLINK
- CMake
- Ninja
- (Optional) ARM Compiler 5 from Keil
- (Optional) Doxygen

### How to install it on Windows
- Install GCC from [Official ARM Distribution](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) and **check "Add to PATH"** when installing
- Install [CMake](https://cmake.org/download/), also **Add to PATH** when installing
- Install [J-Link Software and Documentation pack](https://www.segger.com/downloads/jlink/)
- Download [Ninja](https://github.com/ninja-build/ninja/releases) and put it into some folder in PATH, for example in `C:\Program Files\CMake\bin`
- [Install Visual Studio Code](https://code.visualstudio.com/download) and its extensions
- (Optional) Install [Doxygen](https://www.doxygen.nl/download.html) for generating documentation


### How to install it on other operating systems
- Install the required software from author's website or from OS repositories.


Now you can open the project folder. Select GNU toolchain as CMake kit when asked and edit file `launch.json` inside `.vscode` folder if your `JLinkGDBServer` location is different than he one used in this project. For ARM Compiler 5.06 toolchain,
an environment variable `ARMCC_TOOLCHAIN_PATH` has to be set to `bin` folder of the toolchain files. Alternatively, `armcc.cmake` file inside `toolchain-conf` can be edited to make te path setting permanent.

### Environment features
- two-click switch between ARM Compiler 5 and GCC Compiler
- all code editing features that Visual Studio Code has to offer
- easy debugging of compiled code
- two-click generation of doxygen HTML documentation
- built-in git integration
- works on Windows, GNU/Linux and Mac OS (or at least should)


## HAL
This simple hardware abstraction layer is based on two sources: KL05 Sample Code Package
from Freescale and MBED platform-specific code for KLXX targets. The C API relies
on heavy use of custom enums and structs to ensure that invalid function parameters can be found in
compilation stage. It uses C99 features (`stdint.h`, `stdbool.h`, in-place struct 
initialization etc.) and was tested with GCC 11 compiler and ARM 5.06 Compiler, but should
be compiler-independent, as it does not use compiler-dependent features directly, but instead it's using
CMSIS functions as abstraction layer.


### Goals:
- Prevent user from error-prone and hard-to-debug manual peripheral register setting
- Simple and straightforward for both understanding the underlying code and using the API
- Versatile - use for all applications within this uC
- Smallish footprint
- Separated device drivers from mcu peripheral drivers (exceptions only for very timing-sensitive devices)
- User-proof, mostly
- Peripherals can be reinitialized and will still work
- Communication peripheral drivers can be used without gpio driver
- Supports any MCU working frequency 


### Non-goals:
- universal
- unified (in GPIO mapping, same peripheral but more instances etc.)


### Communication peripheral drivers features:
#### UART:
- different pinout configuration
- automatic calculation for baudrate with ability to re-init if CPU speed changes
- error if configuration baudrate or pins cannot be set
- simple print, putchar and getchar API
- RX IRQ callback setting and checking RX overrun

#### I2C:
- different pinout configuration
- different speed selection
