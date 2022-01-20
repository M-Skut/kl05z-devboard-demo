# CMake file for KL05Z Template Project
cmake_minimum_required(VERSION 3.15.3)

# Print out extra messages to see what is going on. Comment it to have less verbose messages
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Path to toolchain file. This one has to be before 'project()' below
# set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchain-conf/clang-arm-toolchain.cmake)

# Setup project and linker file
project(KL05Z-DevBoardDemo)
set(LINKER_FILE ${CMAKE_SOURCE_DIR}/device/MKL05Z4/MKL05Z4_flash.ld)

# Project-specific options:
option(USE_MINIMAL_LIBC "Use bare implementation of libc" OFF)
 
enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Doxygen target for documentation
set(DOXYGEN_CONF_IN "doxygen.conf")
include("doxygen.cmake")

# For clangd, if used
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Issue a message to be sure it uses the correct toolchain file.
message(STATUS "CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")

# List of source files
set(SRC_FILES
        src/project/main.c

        src/project/error_handler.h
        src/project/cmdline.c
        src/project/cmdline.h

        src/mcu-drivers/gpio.c
        src/mcu-drivers/gpio.h
        src/mcu-drivers/delay.c
        src/mcu-drivers/delay.h
        src/mcu-drivers/uart.c
        src/mcu-drivers/uart.h
        src/mcu-drivers/i2c.c
        src/mcu-drivers/i2c.h
        src/mcu-drivers/spi.c
        src/mcu-drivers/spi.h
        src/mcu-drivers/pwm.c
        src/mcu-drivers/pwm.h       
        src/mcu-drivers/adc.c
        src/mcu-drivers/adc.h 

        src/drivers/mma8451q.c
        src/drivers/mma8451q.h
        src/drivers/ws2812b.c
        src/drivers/ws2812b.h
        src/drivers/u8x8_mkl_hal.c
        src/drivers/u8x8_mkl_hal.h
        
        src/third-party/ftoa/ftoa.c
        src/third-party/ftoa/ftoa.h

        src/third-party/microrl/src/microrl.c
        src/third-party/microrl/src/microrl.h
        src/third-party/microrl/src/config.h

        src/third-party/u8g2/csrc/u8log_u8x8.c
        src/third-party/u8g2/csrc/u8log.c
        src/third-party/u8g2/csrc/u8x8_8x8.c
        src/third-party/u8g2/csrc/u8x8_byte.c
        src/third-party/u8g2/csrc/u8x8_cad.c
        src/third-party/u8g2/csrc/u8x8_capture.c
        src/third-party/u8g2/csrc/u8x8_d_pcf8814_hx1230.c
        src/third-party/u8g2/csrc/u8x8_d_pcd8544_84x48.c
        src/third-party/u8g2/csrc/u8x8_debounce.c
        src/third-party/u8g2/csrc/u8x8_display.c
        src/third-party/u8g2/csrc/u8x8_fonts.c
        src/third-party/u8g2/csrc/u8x8_gpio.c
        src/third-party/u8g2/csrc/u8x8_input_value.c
        src/third-party/u8g2/csrc/u8x8_selection_list.c
        src/third-party/u8g2/csrc/u8x8_setup.c
        src/third-party/u8g2/csrc/u8x8_string.c
        src/third-party/u8g2/csrc/u8x8_u8toa.c
        src/third-party/u8g2/csrc/u8x8_u16toa.c
        src/third-party/u8g2/csrc/u8x8.h

        device/MKL05Z4/system_MKL05Z4.h
        device/MKL05Z4/MKL05Z4.h
        device/MKL05Z4/system_MKL05Z4.c
        device/MKL05Z4/flash_config.c
        device/MKL05Z4/mtb.c

        CMSIS/cmsis_armclang.h
        CMSIS/cmsis_armclang_ltm.h
        CMSIS/cmsis_compiler.h
        CMSIS/cmsis_iccarm.h
        CMSIS/cmsis_armcc.h
        CMSIS/cmsis_version.h
        CMSIS/cmsis_gcc.h
        CMSIS/core_cm0plus.h
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")      
        list(APPEND SRC_FILES "device/MKL05Z4/startup_MKL05Z4.c")
        if (USE_MINIMAL_LIBC)
                message(STATUS "Using minimal libc")
                list(APPEND SRC_FILES "device/MKL05Z4/min_libc.c")
        endif()
else()
        list(APPEND SRC_FILES "device/MKL05Z4/startup_MKL05Z4.s")
endif()


# Build the executable based on the source files
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})

# List of compiler defines, prefix with -D compiler option
target_compile_definitions(${PROJECT_NAME} PRIVATE
        CORE_M0PLUS
        )

# List of include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        CMSIS
        src
        src/project
        src/mcu-drivers
        src/drivers
        src/third-party/microrl/src
        src/third-party/u8g2/csrc
        src/third-party/ftoa
        device/MKL05Z4
        )

# Compiler options
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE
                -mcpu=cortex-m0plus
                -mthumb
                )

        if(USE_MINIMAL_LIBC)
                target_compile_options(${PROJECT_NAME} PRIVATE
                -nostartfiles
                -nostdlib
                )
        endif()

        # Linker options
        target_link_options(${PROJECT_NAME} PRIVATE
                -T${LINKER_FILE}
                -mcpu=cortex-m0plus
                -mthumb
                )
        
        if(USE_MINIMAL_LIBC)
                target_link_options(${PROJECT_NAME} PRIVATE
                -nostartfiles
                -nostdlib
                -lm
                -lgcc
                )
        else()
                target_link_options(${PROJECT_NAME} PRIVATE
                -lc
                -lm
                )
        endif()     
else()
        target_compile_options(${PROJECT_NAME} PRIVATE
                --thumb
                --split_sections
                #--gnu
                --diag_suppress 1
                --apcs=interwork
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                "SHELL:--ro-base 0x00000000"
                "SHELL:--entry 0x00000000"
                "SHELL:--rw-base 0x1FFFFC00"
                "SHELL:--entry Reset_Handler"
                "SHELL:--first __Vectors"
                --remove --strict --summary_stderr --info sizes,totals,unused
        )
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".axf")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # Print executable size as part of the post build process
        add_custom_command(TARGET ${PROJECT_NAME}
                POST_BUILD
                COMMAND ${CMAKE_SIZE_UTIL} ${PROJECT_NAME})

        # Create hex, bin and S-Record files after the build
        add_custom_command(TARGET ${PROJECT_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME} ${PROJECT_NAME}.elf
                COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 ${PROJECT_NAME} ${PROJECT_NAME}.s19
                COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex
                COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin)
else()

        set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
        set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${ARMCC_FROMELF} --text -z ${PROJECT_NAME}.axf
                COMMENT "Print memory info")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${ARMCC_FROMELF} --output ${HEX_FILE} --i32 ${PROJECT_NAME}.axf
                COMMAND ${ARMCC_FROMELF} --output ${BIN_FILE} --bin ${PROJECT_NAME}.axf
                COMMENT "Creating $${PROJECT_NAME}.hex and $${PROJECT_NAME}.bin")
endif()
