
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/**
 * 	enum for status returns
 */
typedef enum
{
	DS_API_STATUS_OK = 0,
	DS_API_STATUS_NOT_INITIALIZED,
	DS_API_STATUS_DEVICE_NOT_FOUND,
	DS_API_STATUS_WRITE_ERROR,
	DS_API_STATUS_READ_ERROR,
	DS_API_STATUS_INVALID_PARAMETERS
} ds_api_status_t;

typedef struct{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_of_week;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} ds_time_data_t;

typedef struct{
	uint16_t temperature;
} ds_temperature_data_t;

ds_api_status_t ds_init				(I2C_HandleTypeDef *hi2c, const uint8_t device_address);
ds_api_status_t ds_read_time		(ds_time_data_t *const time_data);
ds_api_status_t ds_write_time		(const ds_time_data_t *const time_data);
ds_api_status_t ds_read_temperature	(ds_temperature_data_t *const temperature_data);

