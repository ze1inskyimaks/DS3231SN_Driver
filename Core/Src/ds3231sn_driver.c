
#include "ds_api.h"

#define TEMPERATURE_ADDRESS 0x11

typedef struct{
	I2C_HandleTypeDef *hi2c1;
	uint8_t device_address;
	bool is_device_initialized;
} ds_init_data_t;

static ds_init_data_t ds_data = {0};

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

ds_api_status_t ds_write_time(const ds_time_data_t *const time_data){
	return DS_API_STATUS_OK;
}

ds_api_status_t ds_read_temperature(ds_temperature_data_t *const temperature_data){
	ds_api_status_t status = DS_API_STATUS_OK;
	if (NULL == temperature_data) {
		status = DS_API_STATUS_READ_ERROR;
	}
	if (!ds_data.is_device_initialized || HAL_OK != HAL_I2C_IsDeviceReady(ds_data.hi2c1, ds_data.device_address, 5, 10)) {
		status = DS_API_STATUS_NOT_INITIALIZED;
	}

	uint8_t buffer[2] = {0};

	if (DS_API_STATUS_OK == status) {
		if(HAL_OK != HAL_I2C_Mem_Read(ds_data.hi2c1, ds_data.device_address, TEMPERATURE_ADDRESS, I2C_MEMADD_SIZE_8BIT,
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

