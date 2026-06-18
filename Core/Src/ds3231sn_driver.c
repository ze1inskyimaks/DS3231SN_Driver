
#include "ds_api.h"

typedef struct{
	I2C_HandleTypeDef *hi2c;
	uint8_t device_address;
	bool is_device_initialized;
} ds_init_data_t;

static ds_init_data_t ds_data = {0};

ds_api_status_t ds_init(I2C_HandleTypeDef *hi2c, const uint8_t device_address){
	ds_api_status_t status = DS_API_STATUS_OK;
	HAL_StatusTypeDef result;

	if(NULL == hi2c || 0 == device_address || true == ds_data.is_device_initialized){
		status = DS_API_STATUS_INVALID_PARAMETERS;
	}

	if(DS_API_STATUS_OK == status){
		result = HAL_I2C_IsDeviceReady(hi2c, device_address, 5, 10);

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
	return DS_API_STATUS_OK;
}

ds_api_status_t ds_write_time(const ds_time_data_t *const time_data){
	return DS_API_STATUS_OK;
}

ds_api_status_t ds_read_temperature(ds_temperature_data_t *const temperature_data){
	return DS_API_STATUS_OK;
}


