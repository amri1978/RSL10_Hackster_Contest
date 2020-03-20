//-----------------------------------------------------------------------------
// Copyright (c) 2018 Semiconductor Components Industries LLC
// (d/b/a "ON Semiconductor").  All rights reserved.
// This software and/or documentation is licensed by ON Semiconductor under
// limited terms and conditions.  The terms and conditions pertaining to the
// software and/or documentation are available at
// http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf ("ON Semiconductor Standard
// Terms and Conditions of Sale, Section 8 Software") and if applicable the
// software license agreement.  Do not use this software and/or documentation
// unless you have carefully read and you agree to the limited terms and
// conditions.  By using this software and/or documentation, you agree to the
// limited terms and conditions.
//-----------------------------------------------------------------------------
//! \file noa1305.h
//!
//! \mainpage
//!
//! This driver library contains the ON Semiconductor's <b>NOA1305
//! Ambient Light Sensor</b> API
//!
//! Product page:<br>
//! https://www.onsemi.com/PowerSolutions/product.do?id=NOA1305
//!
//! IDK Evaluation board:<br>
//! https://www.onsemi.com/PowerSolutions/evalBoard.do?id=ALS-GEVB
//!
//! API reference:<br>
//! \ref NOA1305_DRIVER_GRP
//!
//! \addtogroup NOA1305_DRIVER_GRP NOA1305 API
//! \{
//-----------------------------------------------------------------------------
#ifndef NOA1305_H_
#define NOA1305_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//! \name Library error codes
//! \{

/** \brief Library call was successful. */
#define NOA1305_OK                     (0)

/** \brief Invalid pointer argument was passed to library function. */
#define NOA1305_E_NULL_PTR             (1)

/** \brief Transfer function indicated communication error. */
#define NOA1305_E_COMM                 (2)

/** \brief Device ID register content does not match expected value. */
#define NOA1305_E_DEV_NOT_FOUND        (3)

/** \brief Argument passed to library function has invalid value. */
#define NOA1305_E_OUT_OF_RANGE         (4)

/** \brief Given operation is not supported. */
#define NOA1305_E_UNSUPPORTED          (5)

/** \brief Success return code for I2C transfer function. */
#define NOA1305_COMM_OK                (0)

/** \brief Failure return code for I2C transfer function. */
#define NOA1305_COMM_ERROR             (1)

//! \}

/** \brief NOA1305 I2C Interface 7-bit address */
#define NOA1305_I2C_ADDR               (0x39)

//! \anchor NOA1305_GRP_I2C_REG_MAP
//! \name I2C Register Map
//! \{

#define NOA1305_REG_POWER_CONTROL      (0x00)
#define NOA1305_REG_RESET              (0x01)
#define NOA1305_REG_INTEGEGRATION_TIME (0x02)
#define NOA1305_REG_INT_SELECT         (0x03)
#define NOA1305_REG_INT_THRESHOLD_LSB  (0x04)
#define NOA1305_REG_INT_THRESHOLD_MSB  (0x05)
#define NOA1305_REG_ALS_DATA_LSB       (0x06)
#define NOA1305_REG_ALS_DATA_MSB       (0x07)
#define NOA1305_REG_DEVICE_ID_LSB      (0x08)
#define NOA1305_REG_DEVICE_ID_MSB      (0x09)

//! \}

//! \anchor NOA1305_DRIVER_GRP_POWER_MODES
//! \name NOA1305 Power Modes
//! \{

/** \brief Sensor is powered down. */
#define NOA1305_POWER_DOWN             (0x00)

/** \brief Sensor is powered up and does periodic measurements.
 *
 * This is the default power mode after power up.
 */
#define NOA1305_POWER_ON               (0x08)

#define NOA1305_TEST_MODE_1            (0x09) /**< Reserved */
#define NOA1305_TEST_MODE_2            (0x0A) /**< Fixed output of 0x5555 */
#define NOA1305_TEST_MODE_3            (0x0B) /**< Fixed output of 0xAAAA */

//! \}

/** \brief Value that needs to be written into the reset register to do
 * software reset of the device.
 */
#define NOA1305_RESET_VAL              (0x10)

//! \anchor NOA1305_DRIVER_GRP_INTEG_TIME
//! \name NOA1305 Integration Time Setting
//! \{

#define NOA1305_INTEG_TIME_800MS       (0x00)
#define NOA1305_INTEG_TIME_400MS       (0x01)
#define NOA1305_INTEG_TIME_200MS       (0x02)
#define NOA1305_INTEG_TIME_100MS       (0x03)
#define NOA1305_INTEG_TIME_50MS        (0x04)
#define NOA1305_INTEG_TIME_25MS        (0x05)
#define NOA1305_INTEG_TIME_12p5MS      (0x06)
#define NOA1305_INTEG_TIME_6p25MS      (0x07)

//! \}

//! \name NOA1305 Interrupt generation Setting
//! \{

/** \brief INT pin is High if measured ALS level is greater then threshold
 * value.
 */
#define NOA1305_INT_LOW_TO_HIGH        (0x01)

/** \brief INT pin is Low if measured ALS level is greater then threshold
 * value.
 */
#define NOA1305_INT_HIGH_TO_LOW        (0x02)

/** \brief INT pin is inactive, always High. */
#define NOA1305_INT_INACTIVE           (0x03)

//! \}

/** \brief NOA1305 Device ID value, LSB */
#define NOA1305_DEVICE_ID_LSB_VAL      (0x19)

/** \brief NOA1305 Device ID value, MSB */
#define NOA1305_DEVICE_ID_MSB_VAL      (0x05)

/** \brief NOA1305 Device ID value */
#define NOA1305_DEVICE_ID              (0x0519)

/** \brief Scaling factor for integration constant stored in noa1305_t
 * structure.
 */
#define NOA1305_INTEG_CONSTANT_DIV     (1000.0f)

/** \brief Control structure for NOA1305 containing platform dependent
 * communication functions and device parameters.
 *
 * \note
 * All member variables of this structure have to be set before it can be used
 * with the NOA1305 API functions.
 */
struct noa1305_t
{
	int32_t (*read_func)(uint8_t dev_id, uint8_t addr, uint8_t *value);
	int32_t (*write_func)(uint8_t dev_id, uint8_t addr, uint8_t value);
	void (*delay_func)(uint32_t ms);

	/** \brief I_K times 1000 */
	int32_t integration_constatnt;

	/** \brief Device id passed to bus read / write functions.
	 *
	 * Needs to be set before calling noa1305_init function.
	 */
	uint8_t id;
};

/** \brief Initializes NOA1305 Ambient Light Sensor.
 *
 * This function does the following steps:
 *
 * 1. Issues Software reset command to NOA1305.
 * 2. Tests for correct Device ID value.
 * 3. Turns the sensor into power off mode.
 *
 * \param dev
 * Pointer to NOA1305 control structure.
 * This structure must have all of its member variables set to platform specific
 * communication functions before calling this function.
 *
 * \returns
 * \ref NOA1305_OK - On success.<br>
 * \ref NOA1305_E_DEV_NOT_FOUND - Device ID mismatch.<br>
 * \ref NOA1305_E_NULL_PTR - \p dev pointer or its value are invalid.
 * \b NOA1305_E_* - Error code on error.
 */
extern int32_t noa1305_init(struct noa1305_t *dev);

/** \brief Set NOA1305 power mode.
 *
 * \param power_mode
 * One of available \ref NOA1305_GRP_POWER_MODES.
 *
 * \param dev
 * Pointer to initialized NOA1305 control structure.
 *
 * \returns
 * \ref NOA1305_OK - On success.<br>
 * \b NOA1305_E_* - Error code on error.
 */
extern int32_t noa1305_set_power_mode(uint8_t power_mode, struct noa1305_t *dev);

/** \brief Read current NOA1305 power mode.
 *
 * \param power_mode
 * Pointer to variable where read poer mode will be saved.
 *
 * \param dev
 * Pointer to initialized NOA1305 control structure.
 *
 * \returns
 * \ref NOA1305_OK - On success.<br>
 * \b NOA1305_E_* - Error code on error.
 */
extern int32_t noa1305_get_power_mode(uint8_t *power_mode,
		struct noa1305_t *dev);

/** \brief Issues a software reset command to NOA1305.
 *
 * \param dev
 * Pointer to initialized NOA1305 control structure.
 *
 * \returns
 * \ref NOA1305_OK - On success.<br>
 * \b NOA1305_E_* - Error code on error.
 */
extern int32_t noa1305_reset(struct noa1305_t *dev);

/** \brief Write new integration time setting to NOA1305. */
extern int32_t noa1305_set_integration_time(uint8_t integ_time,
		struct noa1305_t *dev);

extern int32_t noa1305_get_integration_time(uint8_t *integ_time,
		struct noa1305_t *dev);

/** \brief Write new interrupt select setting to NOA1305. */
extern int32_t noa1305_set_int_select(uint8_t int_select, struct noa1305_t *dev);

extern int32_t noa1305_get_int_select(uint8_t *int_select,
		struct noa1305_t *dev);

/** \brief Write new interrupt threshold value setting to NOA1305. */
extern int32_t noa1305_set_int_threshold(uint16_t threshold,
		struct noa1305_t *dev);

extern int32_t noa1305_get_int_threshold(uint16_t *threshold,
		struct noa1305_t *dev);

/** \brief Read latest ambient light measurement from NOA1305. */
extern int32_t noa1305_read_als_data(uint16_t *als_data, struct noa1305_t *dev);

/** \brief Read device ID from NOA1305. */
extern int32_t noa1305_read_device_id(uint16_t *dev_id, struct noa1305_t *dev);

/** \brief Read latest ambient light measurement from NOA1305 and convert it
 * into Lux.
 *
 * This function uses integer arithmetic and has the precision of 1 Lux.
 *
 * \param lux
 * Pointer to variable where the read ambient light value in Lux will be
 * stored.
 *
 * \param dev
 * Pointer to initialized NOA1305 control structure.
 *
 * \returns
 * \ref NOA1305_OK - On success.<br>
 * \b NOA1305_E_* - Error code on error.
 */
extern int32_t noa1305_convert_als_data_lux(uint32_t *lux,
		struct noa1305_t *dev);

/** \brief Write new interrupt threshold value into NOA1305 based on Lux value.
 *
 * This function uses integer arithmetic and has the precision of 1 Lux.
 *
 * \param lux
 * Ambient light value in Lux that will be converted and written into the
 * threshold register.
 *
 * \param dev
 * Pointer to initialized NOA1305 control structure.
 *
 * \returns
 * \ref NOA1305_OK - On success.<br>
 * \b NOA1305_E_* - Error code on error.
 */
extern int32_t noa1305_set_int_threshold_lux(uint32_t lux,
		struct noa1305_t *dev);

/** \brief Reads current interrupt threshold value from NOA1305 and converts it
 * into Lux.
 *
 * \param lux
 * Pointer to variable where the read ambient light value in Lux will be
 * stored.
 *
 * \param dev
 * Pointer to initialized NOA1305 control structure.
 *
 * \returns
 * \ref NOA1305_OK - On success.<br>
 * \b NOA1305_E_* - Error code on error.
 */
extern int32_t noa1305_get_int_threshold_lux(uint32_t *lux,
		struct noa1305_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NOA1305_H_ */

//! \}
