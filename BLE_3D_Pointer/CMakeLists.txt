
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_CXX_COMPILER arm-none-eabi-g++)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)

include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/bdk)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/BDK/1.8.0/include/components)
include_directories(${PROJECT_SOURCE_DIR}/BDK/1.8.0/include/components/bhy1_firmware)
include_directories(${PROJECT_SOURCE_DIR}/RSL10/3.0.534/include)
include_directories(${PROJECT_SOURCE_DIR}/RSL10/3.0.534/include/bb)
include_directories(${PROJECT_SOURCE_DIR}/RSL10/3.0.534/include/ble)
include_directories(${PROJECT_SOURCE_DIR}/RSL10/3.0.534/include/ble/profiles)
include_directories(${PROJECT_SOURCE_DIR}/RSL10/3.0.534/include/kernel)
include_directories(${PROJECT_SOURCE_DIR}/RTE)
include_directories(${PROJECT_SOURCE_DIR}/RTE/Device/RSL10)


SET(CMAKE_C_FLAGS "-Wall -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DRSL10_CID=101 -DCFG_CON=8 -DCFG_BLE=1 -DCFG_SLEEP -DCFG_ALLROLES=0 -DCFG_APP -DCFG_APP_BATT -DCFG_ATTS=1 -DCFG_EMB=1 -DCFG_HOST=1 -DCFG_RF_ATLAS=1 -DCFG_ALLPRF=1 -DCFG_PRF=1 -DCFG_NB_PRF=2 -DCFG_CHNL_ASSESS=1 -DCFG_SEC_CON=1 -DCFG_EXT_DB -DCFG_PRF_BASS=1 -D_RTE_ -std=gnu11 -Wmissing-prototypes")

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-mcpu=cortex-m3 -mthumb -mlittle-endian -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wshadow  -g3 -T ${PROJECT_SOURCE_DIR}/sections.ld -nostartfiles -Xlinker --gc-sections --specs=nano.specs -u _printf_float --specs=nosys.specs")

SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "-mcpu=cortex-m3 -mthumb -mlittle-endian -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wshadow  -g3 -T ${PROJECT_SOURCE_DIR}/sections.ld -nostartfiles -Xlinker --gc-sections --specs=nano.specs -u _printf_float --specs=nosys.specs")

set(CMAKE_C_LINK_EXECUTABLE "arm-none-eabi-gcc <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

set_property(SOURCE RTE/Device/RSL10/startup_rsl10.S PROPERTY LANGUAGE C)
set_property(SOURCE RTE/Device/RSL10/startup_rsl10.S PROPERTY COMPILE_FLAGS "-mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -D_RTE_")
set_property(SOURCE src/wakeup_asm.S PROPERTY LANGUAGE C)
set_property(SOURCE src/wakeup_asm.S PROPERTY COMPILE_FLAGS "-mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -D_RTE_")
add_executable(Atmosphere_Project.elf "RSL10/3.0.534/source/firmware/cmsis/source/sbrk.c" "RSL10/3.0.534/source/firmware/cmsis/source/start.c" "RSL10/3.0.534/source/firmware/ble_abstraction_layer/ble/source/stubprf.c" "RTE/Device/RSL10/startup_rsl10.S" "RTE/Device/RSL10/system_rsl10.c" "adc/adc.c" "app_src/atmosphere_abilityHandler.c" "app_src/atmosphere_callbacks.c" "app_src/atmosphere_elementSetup.c" "app_src/atmosphere_interruptsHandler.c" "app_src/atmosphere_platform.c" "app_src/atmosphere_triggerHandler.c" "app_src/atmosphere_variantSetup.c" "atmo/atmo_strtof.c" "atmo/core.c" "atmo/tinyprintf.c" "base64/atmo_base64.c" "bhi160/bhi160.c" "bhi160/bhy.c" "bhi160/bhy1_fw.c" "bhi160/bhy_support.c" "bhi160/bhy_uc_driver.c" "ble/ble.c" "ble/ble_onsemi.c" "ble/ble_onsemi_db.c" "block/block.c" "block/block_onsemi.c" "bme680/bme680.c" "bme680/bme680_reg.c" "cellular/cellular.c" "cloud/cloud.c" "cloud/cloud_ble.c" "cloud/cloud_provisioner.c" "cloud/cloud_tcp.c" "cloud/cloud_uart.c" "counter/counter_atmo.c" "datetime/datetime.c" "filesystem/filesystem.c" "filesystem/filesystem_crastfs.c" "filesystem/filesystem_lfs.c" "filesystem/lfs.c" "filesystem/lfs_util.c" "gpio/gpio.c" "gpio/gpio_onsemi.c" "http/http.c" "http/picohttpparser.c" "i2c/i2c.c" "i2c/i2c_onsemi.c" "interval/interval.c" "interval/interval_default.c" "interval/interval_onsemi.c" "nfc/nfc.c" "noa1305/noa1305.c" "noa1305/noa1305_onsemi.c" "pwm/pwm.c" "ringbuffer/atmosphere_ringbuffer.c" "spi/spi.c" "src/HAL_RTC.c" "src/app.c" "src/app_ble_hooks.c" "src/app_init.c" "src/app_sleep.c" "src/app_timer.c" "src/app_trace.c" "src/ble/BLE_BASS.c" "src/ble/BLE_ICS.c" "src/ble/BLE_PeripheralServer.c" "src/bsp/I2CEeprom.c" "src/bsp/led_api.c" "src/calibration.c" "src/device/BDK.c" "src/device/BDK_Task.c" "src/device/EventCallback.c" "src/device/HAL.c" "src/device/HAL_I2C.c" "src/device/HAL_clock.c" "src/device/HAL_error.c" "src/device/I2C_RSLxx.c" "src/device/SEGGER_RTT.c" "src/device/SEGGER_RTT_printf.c" "src/device/SoftwareTimer.c" "src/device/stimer.c" "src/wakeup_asm.S" "tcpclient/tcpclient.c" "tcpserver/tcpserver.c" "uart/regex.c" "uart/uart.c" "wifi/wifi.c")



add_custom_command(TARGET Atmosphere_Project.elf POST_BUILD COMMAND arm-none-eabi-objcopy -O ihex "Atmosphere_Project.elf" "Atmosphere_Project.hex" WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Converting to Hex...")

target_link_libraries(Atmosphere_Project.elf ${CMAKE_SOURCE_DIR}/BDK/1.8.0/source/firmware/Components/BSEC_1.4.7.1_Generic_Release_20180907/algo/bin/Normal_version/gcc/Cortex_M3/libalgobsec.a)
target_link_libraries(Atmosphere_Project.elf ${CMAKE_SOURCE_DIR}/RSL10/3.0.534/lib/ble_core/Release/libblelib.a)
target_link_libraries(Atmosphere_Project.elf ${CMAKE_SOURCE_DIR}/RSL10/3.0.534/lib/ble_core/Release/libkelib.a)
target_link_libraries(Atmosphere_Project.elf ${CMAKE_SOURCE_DIR}/RSL10/3.0.534/lib/ble_profiles/Release/libbass.a)
target_link_libraries(Atmosphere_Project.elf ${CMAKE_SOURCE_DIR}/RSL10/3.0.534/lib/Release/libsyslib.a)
