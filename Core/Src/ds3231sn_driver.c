
#include "ds_api.h"

typedef struct{
	I2C_HandleTypeDef *hi2c1;
	uint8_t device_address;
	bool is_device_initialized;
} ds_init_data_t;

static ds_init_data_t ds_data = {0};

static uint8_t bcd_to_decimal(uint8_t bcd);

ds_api_status_t ds_init(I2C_HandleTypeDef *hi2c1, const uint8_t device_address){
	HAL_I2C_IsDeviceReady(hi2c1, device_address, 5, 10);

	ds_data.hi2c1 = hi2c1;
	ds_data.device_address = device_address;
	ds_data.is_device_initialized = true;

	return DS_API_STATUS_OK;
}

ds_api_status_t ds_read_time(ds_time_data_t *const time_data){

	if (time_data == NULL)
	{
		return DS_API_STATUS_INVALID_PARAMETERS;
	}

	if (!ds_data.is_device_initialized)
	{
		return DS_API_STATUS_NOT_INITIALIZED;
	}

	uint8_t reg_address = 0x00;
	uint8_t buffer[7];

	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(
			ds_data.hi2c1,
			ds_data.device_address,
			&reg_address,
			1,
			100);

	if (status != HAL_OK)
	{
		return DS_API_STATUS_READ_ERROR;
	}

	status = HAL_I2C_Master_Receive(
			ds_data.hi2c1,
			ds_data.device_address,
			buffer,
			7,
			100);

	if (status != HAL_OK)
	{
		return DS_API_STATUS_READ_ERROR;
	}


	time_data->seconds = bcd_to_decimal(buffer[0] & 0x7F);

	time_data->minutes = bcd_to_decimal(buffer[1]);

	uint8_t raw_hours = buffer[2];

	if (raw_hours & (1 << 6))
	{
	    uint8_t hour = raw_hours & 0x1F;
	    hour = bcd_to_decimal(hour);

	    bool is_pm = raw_hours & (1 << 5);

	    if (is_pm && hour != 12)
	    {
	        hour += 12;
	    }
	    else if (!is_pm && hour == 12)
	    {
	        hour = 0;
	    }

	    time_data->hours = hour;
	}
	else
	{
	    time_data->hours = bcd_to_decimal(raw_hours & 0x3F);
	}

	time_data->day_of_week = bcd_to_decimal(buffer[3]);

	time_data->day = bcd_to_decimal(buffer[4]);

	time_data->month = bcd_to_decimal(buffer[5] & 0x1F);

	time_data->year = 2000 + bcd_to_decimal(buffer[6]);

	return DS_API_STATUS_OK;
}

ds_api_status_t ds_write_time(const ds_time_data_t *const time_data){
	return DS_API_STATUS_OK;
}

ds_api_status_t ds_read_temperature(ds_temperature_data_t *const temperature_data){
	return DS_API_STATUS_OK;
}

static uint8_t bcd_to_decimal(uint8_t bcd)
{
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

