//-----------------------------------------------------------------------------
// Copyright (c,struct noa1305_t *dev) 2018 Semiconductor Components Industries LLC
// (d/b/a "ON Semiconductor",struct noa1305_t *dev).  All rights reserved.
// This software and/or documentation is licensed by ON Semiconductor under
// limited terms and conditions.  The terms and conditions pertaining to the
// software and/or documentation are available at
// http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf ("ON Semiconductor Standard
// Terms and Conditions of Sale, Section 8 Software",struct noa1305_t *dev) and if applicable the
// software license agreement.  Do not use this software and/or documentation
// unless you have carefully read and you agree to the limited terms and
// conditions.  By using this software and/or documentation, you agree to the
// limited terms and conditions.
//-----------------------------------------------------------------------------
//! \file noa1305.c
//!
//! \addtogroup NOA1305_DRIVER_GRP
//! \{
//-----------------------------------------------------------------------------
#include "noa1305_onsemi.h"

#include <stdlib.h>

/** \brief COnversion constants used to convert measured ALS counts into LUX
 * value for different integration time settings.
 */
static const uint32_t noa1305_conv_param[] = {
		NOA1305_INTEG_CONSTANT_DIV / 0.800f,
		NOA1305_INTEG_CONSTANT_DIV / 0.400f,
		NOA1305_INTEG_CONSTANT_DIV / 0.200f,
		NOA1305_INTEG_CONSTANT_DIV / 0.100f,
		NOA1305_INTEG_CONSTANT_DIV / 0.050f,
		NOA1305_INTEG_CONSTANT_DIV / 0.025f,
		NOA1305_INTEG_CONSTANT_DIV / 0.0125f,
		NOA1305_INTEG_CONSTANT_DIV / 0.00625f
};

/** \brief Converts measured count value into LUX.
 *
 * \param cnt
 * Measured ALS count sample read from NOA1305 ALS output registers.
 *
 * \param ti
 * Integration time used to measure \p cnt sample.
 * Use one of <b>NOA1305_INTEG_TIME_[ 800MS | 400 MS | 200MS | 100MS | 50MS |
 * 25MS | 12p5MS | 6p25MS ]</b>.
 *
 * \param ik
 * Integration constant times 1000.
 *
 * \returns
 * Lux value equivalent to measured \p cnt.
 */
static uint32_t noa1305_cnt_to_lux(uint16_t cnt, uint8_t ti, uint32_t ik)
{
	return (((uint32_t) cnt) * noa1305_conv_param[ti]) / ik;
}

/** \brief Converts Lux value into NOA1305 sensor count value that cen be loaded
 * into interrupt threshold register.
 *
 * \param lux
 * Ambient light in Lux to convert to NOA1305 sensor count.
 *
 * \param ti
 * Integration time used by the sensor.
 * Use one of <b>NOA1305_INTEG_TIME_[ 800MS | 400 MS | 200MS | 100MS | 50MS |
 * 25MS | 12p5MS | 6p25MS ]</b>.
 *
 * \param ik
 * Integration constant times 1000.
 *
 * \returns
 * NOA1305 sensor count equivalent to \p lux value.
 */
static uint16_t noa1305_lux_to_cnt(uint32_t lux, uint8_t ti, uint32_t ik)
{
	return ((uint32_t) (lux * ik) / noa1305_conv_param[ti]);
}

int32_t noa1305_init(struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	if (dev != NULL && dev->read_func != NULL && dev->write_func != NULL
			&& dev->delay_func != NULL)
	{
		retval = noa1305_reset(dev);
		if (retval == NOA1305_OK)
		{
			dev->delay_func(2);

			uint16_t device_id;
			retval = noa1305_read_device_id(&device_id, dev);
			if (retval == NOA1305_OK)
			{
				if (device_id == NOA1305_DEVICE_ID)
				{
					retval = noa1305_set_power_mode(NOA1305_POWER_DOWN, dev);
					dev->delay_func(2);
				}
				else
				{
					retval = NOA1305_E_DEV_NOT_FOUND;
				}
			}
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_set_power_mode(uint8_t power_mode, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	if (dev != NULL)
	{
		if (power_mode == NOA1305_POWER_DOWN
				|| (power_mode >= NOA1305_POWER_ON
						&& power_mode <= NOA1305_TEST_MODE_3))
		{
			retval = dev->write_func(dev->id, NOA1305_REG_POWER_CONTROL,
					power_mode);
			if (retval != NOA1305_COMM_OK)
			{
				retval = NOA1305_E_COMM;
			}
		}
		else
		{
			retval = NOA1305_E_OUT_OF_RANGE;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_get_power_mode(uint8_t *power_mode, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	if (dev != NULL && power_mode != NULL)
	{
		retval = dev->read_func(dev->id, NOA1305_REG_POWER_CONTROL, power_mode);
		if (retval != NOA1305_COMM_OK)
		{
			retval = NOA1305_E_COMM;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_reset(struct noa1305_t *dev)
{
	int32_t retval;

	if (dev != NULL)
	{
		retval = dev->write_func(dev->id, NOA1305_REG_RESET, NOA1305_RESET_VAL);
		if (retval != NOA1305_COMM_OK)
		{
			retval = NOA1305_E_COMM;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_set_integration_time(uint8_t integ_time, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	if (dev != NULL)
	{
		if (integ_time <= NOA1305_INTEG_TIME_6p25MS)
		{
			retval = dev->write_func(dev->id, NOA1305_REG_INTEGEGRATION_TIME,
					integ_time);
			if (retval != NOA1305_COMM_OK)
			{
				retval = NOA1305_E_COMM;
			}
		}
		else
		{
			retval = NOA1305_E_OUT_OF_RANGE;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_get_integration_time(uint8_t *integ_time, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	if (dev != NULL && integ_time != NULL)
	{
		retval = dev->read_func(dev->id, NOA1305_REG_INTEGEGRATION_TIME,
				integ_time);
		if (retval != NOA1305_COMM_OK)
		{
			retval = NOA1305_E_COMM;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_set_int_select(uint8_t int_select, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	if (dev != NULL)
	{
		if (int_select >= NOA1305_INT_LOW_TO_HIGH
				&& int_select <= NOA1305_INT_INACTIVE)
		{
			retval = dev->write_func(dev->id, NOA1305_REG_INT_SELECT,
					int_select);
			if (retval != NOA1305_COMM_OK)
			{
				retval = NOA1305_E_COMM;
			}
		}
		else
		{
			retval = NOA1305_E_OUT_OF_RANGE;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_get_int_select(uint8_t *int_select, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	if (dev != NULL && int_select != NULL)
	{
		retval = dev->read_func(dev->id, NOA1305_REG_INT_SELECT, int_select);
		if (retval != NOA1305_COMM_OK)
		{
			retval = NOA1305_E_COMM;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_set_int_threshold(uint16_t threshold, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	uint8_t thresh_msb = (threshold >> 8) & 0x00FF;
	uint8_t thresh_lsb = threshold & 0x00FF;

	if (dev != NULL)
	{
		retval = dev->write_func(dev->id, NOA1305_REG_INT_THRESHOLD_LSB,
				thresh_lsb);
		if (retval == NOA1305_COMM_OK)
		{
			retval = dev->write_func(dev->id, NOA1305_REG_INT_THRESHOLD_MSB,
					thresh_msb);
			if (retval != NOA1305_COMM_OK)
			{
				retval = NOA1305_E_COMM;
			}
		}
		else
		{
			retval = NOA1305_E_COMM;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_get_int_threshold(uint16_t *threshold, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	uint8_t thresh_msb, thresh_lsb;

	if (dev != NULL && threshold != NULL)
	{
		retval = dev->read_func(dev->id, NOA1305_REG_INT_THRESHOLD_LSB,
				&thresh_lsb);
		if (retval == NOA1305_COMM_OK)
		{
			retval = dev->read_func(dev->id, NOA1305_REG_INT_THRESHOLD_MSB,
					&thresh_msb);
			if (retval == NOA1305_COMM_OK)
			{
				*threshold = (((uint16_t)thresh_msb) << 8) | thresh_lsb;
			}
			else
			{
				retval = NOA1305_E_COMM;
			}
		}
		else
		{
			retval = NOA1305_E_COMM;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_read_als_data(uint16_t *als_data, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	uint8_t data_msb, data_lsb;

	if (dev != NULL && als_data != NULL)
	{
		retval = dev->read_func(dev->id, NOA1305_REG_ALS_DATA_LSB, &data_lsb);
		if (retval == NOA1305_COMM_OK)
		{
			retval = dev->read_func(dev->id, NOA1305_REG_ALS_DATA_MSB,
					&data_msb);
			if (retval == NOA1305_COMM_OK)
			{
				*als_data = (((uint16_t)data_msb) << 8) | data_lsb;
			}
			else
			{
				retval = NOA1305_E_COMM;
			}
		}
		else
		{
			retval = NOA1305_E_COMM;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_read_device_id(uint16_t *dev_id, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	uint8_t id_msb, id_lsb;

	if (dev != NULL && dev_id != NULL)
	{
		retval = dev->read_func(dev->id, NOA1305_REG_DEVICE_ID_LSB, &id_lsb);
		if (retval == NOA1305_COMM_OK)
		{
			retval = dev->read_func(dev->id, NOA1305_REG_DEVICE_ID_MSB,
					&id_msb);
			if (retval == NOA1305_COMM_OK)
			{
				*dev_id = (((uint16_t)id_msb) << 8) | id_lsb;
			}
			else
			{
				retval = NOA1305_E_COMM;
			}
		}
		else
		{
			retval = NOA1305_E_COMM;
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_convert_als_data_lux(uint32_t *lux, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;
	uint16_t als_data;
	uint8_t ti_setting; /* integreation time reg value */

	if (dev != NULL && lux != NULL)
	{
		retval = noa1305_read_als_data(&als_data, dev);
		if (retval == NOA1305_OK)
		{
			retval = noa1305_get_integration_time(&ti_setting, dev);
			if (retval == NOA1305_OK)
			{
				*lux = noa1305_cnt_to_lux(als_data, ti_setting,
						dev->integration_constatnt);
			}
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_set_int_threshold_lux(uint32_t lux, struct noa1305_t *dev)
{
	int32_t retval = NOA1305_OK;

	uint8_t ti_setting = 0;
	uint16_t new_threshold = 0;

	if (dev != NULL)
	{
		retval = noa1305_get_integration_time(&ti_setting, dev);
		if (retval == NOA1305_OK)
		{
			new_threshold = noa1305_lux_to_cnt(lux, ti_setting,
					dev->integration_constatnt);

			retval = noa1305_set_int_threshold(new_threshold, dev);
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

int32_t noa1305_get_int_threshold_lux(uint32_t *lux, struct noa1305_t *dev)
{
	int retval = NOA1305_OK;

	uint8_t ti_setting = 0;
	uint16_t threshold = 0;

	if (dev != NULL && lux != NULL)
	{
		retval = noa1305_get_integration_time(&ti_setting, dev);
		if (retval == NOA1305_OK)
		{
			retval = noa1305_get_int_threshold(&threshold, dev);
			if (retval == NOA1305_OK)
			{
				*lux = noa1305_cnt_to_lux(threshold, ti_setting,
						dev->integration_constatnt);
			}
		}
	}
	else
	{
		retval = NOA1305_E_NULL_PTR;
	}

	return retval;
}

//! \}
