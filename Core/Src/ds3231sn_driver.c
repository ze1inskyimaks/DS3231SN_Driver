
#include "ds_api.h"

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
	return DS_API_STATUS_OK;
}

