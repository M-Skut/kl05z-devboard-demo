# KL-05 DevBoard Demo project with simple HAL

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
- (Optionally) Install [Doxygen](https://www.doxygen.nl/download.html) for generating documentation

Now you can open the project folder. Select GNU toolchain as CMake kit when asked and edit file `launch.json` inside `.vscode` folder if your JLinkGDBServer location is different than used in the project.

### Enviroment features
- two-click swich between ARM Compiler 5 and GCC Compiler
- all code editing features that Visual Studio Code offers
- easy debugging of compiled code
- two-click generation of doxygen documentation
- built-in git integration
- works on Windows, GNU/Linux and Mac OS (or at least should)


## HAL
This simple hardware abstraction layer is based on two sources: KL05 Sample Code Package
from Freescale and MBED platform-specific code for KLXX targets. It's C API that relies
on use of custom enums and structs to ensure that invalid parameters can be found in
compilation stage. Uses C99 features (`stdint.h`, `stdbool.h`, in-place struct 
initialization etc.) and was tested with GCC 12 compiler and ARM 5.06 Compiler, but should
be compiler-independent a it does not use compiler-dependent features directly, only using
CMSIS 5 as abstraction layer.


### Goals:
- Prevent user from error-prone and hard-to-debug manual peripheral register setting
- Simple and straightforward for both reading and using
- Versatile - use for all applications within this uC
- Smallish footprint
- Separated device drivers from mcu-drivers (exceptions only for very timing-sensitive devices)
- User-proof, mostly
- Can be reinitialized and will still work
- Communication peripheral drivers can be used without gpio driver
- Support for changing MCU working frequency


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
