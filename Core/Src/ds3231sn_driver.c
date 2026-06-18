
#include "ds_api.h"

#define TEMPERATURE_ADDRESS 0x11
#define START_TIME_ADDRESS 0x00
#define TRIALS 5
#define TIMEOUT 10


typedef struct{
	I2C_HandleTypeDef *hi2c;
	uint8_t device_address;
	bool is_device_initialized;
} ds_init_data_t;

static ds_init_data_t ds_data = {0};

static uint8_t bcd_to_decimal(uint8_t bcd);

static uint8_t decimal_to_bcd(const uint8_t decimal);

static uint8_t hours_decimal_to_bcd_12format(const uint8_t decimal_hours);

static inline bool is_valid_parameters(void *data);
static inline bool is_device_initialized();


ds_api_status_t ds_init(I2C_HandleTypeDef *hi2c, const uint8_t device_address){
	ds_api_status_t status = DS_API_STATUS_OK;
	HAL_StatusTypeDef result;

	if(NULL == hi2c || 0 == device_address || true == ds_data.is_device_initialized){
		status = DS_API_STATUS_INVALID_PARAMETERS;
	}

	if(DS_API_STATUS_OK == status){
		result = HAL_I2C_IsDeviceReady(hi2c, device_address, TRIALS, TIMEOUT);

		if(HAL_OK != result) {
			status = DS_API_STATUS_DEVICE_NOT_FOUND;
		}
	}

	if(DS_API_STATUS_OK == status)
	{
		ds_data.hi2c = hi2c;
		ds_data.device_address = device_address;
		ds_data.is_device_initialized = true;
	}

	return status;
}

ds_api_status_t ds_read_time(ds_time_data_t *const time_data){
	ds_api_status_t retcode = DS_API_STATUS_OK;

	if (!is_valid_parameters((void *)time_data)) {
		retcode = DS_API_STATUS_INVALID_PARAMETERS;
	}

	if (!is_device_initialized()) {
		retcode = DS_API_STATUS_DEVICE_NOT_FOUND;
	}

	uint8_t buffer[7];

	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read(

			ds_data.hi2c,
			ds_data.device_address,
			START_TIME_ADDRESS,
			I2C_MEMADD_SIZE_8BIT,
			buffer,
			7,
			HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		retcode = DS_API_STATUS_READ_ERROR;
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

	retcode = DS_API_STATUS_OK;

	return retcode;
}

ds_api_status_t ds_write_time(const ds_time_data_t *const time_data, const bool is_24_hour_format){
	ds_api_status_t retcode = DS_API_STATUS_OK;

	if (!is_valid_parameters((void *)time_data)) {
		retcode = DS_API_STATUS_INVALID_PARAMETERS;
	}
	if (!is_device_initialized()) {
		retcode = DS_API_STATUS_DEVICE_NOT_FOUND;
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
				ds_data.hi2c,
				ds_data.device_address,
				START_TIME_ADDRESS,
				I2C_MEMADD_SIZE_8BIT,
				(uint8_t*)&write_data,
				sizeof(write_data),
				HAL_MAX_DELAY))
		{
			retcode = DS_API_STATUS_WRITE_ERROR;
		}
	}

	return retcode;
}

ds_api_status_t ds_read_temperature(ds_temperature_data_t *const temperature_data){
	ds_api_status_t status = DS_API_STATUS_OK;

	if (!is_valid_parameters(temperature_data)) {
		status = DS_API_STATUS_INVALID_PARAMETERS;
	}
	if (!is_device_initialized()) {
		status = DS_API_STATUS_DEVICE_NOT_FOUND;
	}

	uint8_t buffer[2] = {0};

	if (DS_API_STATUS_OK == status) {
		if(HAL_OK != HAL_I2C_Mem_Read(ds_data.hi2c, ds_data.device_address, TEMPERATURE_ADDRESS, I2C_MEMADD_SIZE_8BIT,
				buffer, sizeof(buffer), HAL_MAX_DELAY)) {
			status = DS_API_STATUS_READ_ERROR;
		} else {
			int16_t temp_raw = (int16_t)((buffer[0] << 8) | buffer[1]);
			temperature_data->temperature = (temp_raw >> 6) * 0.25;
			status = DS_API_STATUS_OK;
		}

	}
	return status;
}

static uint8_t decimal_to_bcd(const uint8_t decimal) {
	uint8_t bcd = 0;
	bcd = (decimal / 10) << 4;
	bcd |= decimal % 10;

	return bcd;
}
  
static uint8_t bcd_to_decimal(uint8_t bcd)
{
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
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

static inline bool is_valid_parameters(void *data) {
	bool retcode = true;
	if (NULL == data) {
		retcode = false;
	}
	return retcode;
}

static inline bool is_device_initialized() {
	bool retcode = true;

	if (!ds_data.is_device_initialized || HAL_OK != HAL_I2C_IsDeviceReady(ds_data.hi2c, ds_data.device_address, TRIALS, TIMEOUT)) {
			retcode = false;
		}
	return retcode;
}
