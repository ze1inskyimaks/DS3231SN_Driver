
#include "ds_api.h"

typedef struct{
	I2C_HandleTypeDef *hi2c1;
	uint8_t device_address;
	bool is_device_initialized;
} ds_init_data_t;

static ds_init_data_t ds_data = {0};

static uint8_t decimal_to_bcd(const uint8_t decimal);

static uint8_t hours_decimal_to_bcd_12format(const uint8_t decimal_hours)

ds_api_status_t ds_init(I2C_HandleTypeDef *hi2c1, const uint8_t device_address){
	HAL_I2C_IsDeviceReady(hi2c1, device_address, 5, 10);

	ds_data.hi2c1 = hi2c1;
	ds_data.device_address = device_address;
	ds_data.is_device_initialized = true;

	return DS_API_STATUS_OK;
}

ds_api_status_t ds_read_time(ds_time_data_t *const time_data){
	return DS_API_STATUS_OK;
}

ds_api_status_t ds_write_time(const ds_time_data_t *const time_data, const bool is_24_hour_format){
	ds_api_status_t retcode = DS_API_STATUS_OK;

	if (NULL == time_data) {
		retcode = DS_API_STATUS_INVALID_PARAMETERS;
	}

	if (DS_API_STATUS_OK == retcode && HAL_OK != HAL_I2C_IsDeviceReady(ds_data.hi2c1, ds_data.device_address, 5, 10)) {
		retcode = DS_API_STATUS_DEVICE_NOT_FOUND;
	}

	if (DS_API_STATUS_OK == retcode && !ds_data.is_device_initialized) {
		retcode = DS_API_STATUS_NOT_INITIALIZED;
	}

	if (DS_API_STATUS_OK == retcode) {
		ds_time_data_t write_data = {0};

		write_data.seconds = decimal_to_bcd(time_data->seconds);
		write_data.minutes = decimal_to_bcd(time_data->minutes);

		if (is_24_hour_format) {
			write_data.hours = decimal_to_bcd(time_data->hours);
		} else {
			write_data.hours = hours_decimal_to_bcd_12format(time_data->hours);
		}

		write_data.day_of_week = decimal_to_bcd(time_data->day_of_week);
		write_data.day = decimal_to_bcd(time_data->day);
		write_data.month = decimal_to_bcd(time_data->month);
		write_data.year = decimal_to_bcd(time_data->year - 2000);

		if (HAL_OK !=  HAL_I2C_Mem_Write(
				ds_data.hi2c1,
				ds_data.device_address,
				0x00,
				I2C_MEMADD_SIZE_8BIT,
				(uint8_t*)&write_data,
				sizeof(write_data),
				100))
		{
			retcode = DS_API_STATUS_WRITE_ERROR;
		}
	}

	return retcode;
}

ds_api_status_t ds_read_temperature(ds_temperature_data_t *const temperature_data){
	return DS_API_STATUS_OK;
}

static uint8_t decimal_to_bcd(const uint8_t decimal) {
	uint8_t bcd = 0;
	bcd = (decimal / 10) << 4;
	bcd |= decimal % 10;

	return bcd;
}

static uint8_t hours_decimal_to_bcd_12format(const uint8_t decimal_hours) {
	uint8_t bcd = 0;
	bcd |= 1 << 6;

	if (decimal_hours > 12) {
		bcd |= 1 << 5;
		bcd |= decimal_to_bcd(decimal_hours - 12);
	} else {
		bcd |= decimal_to_bcd(decimal_hours);
	}

	return bcd;
}

