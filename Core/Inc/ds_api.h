
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/**
 * @brief Status codes returned by DS3231 driver operations.
 */
typedef enum
{
	DS_API_STATUS_OK = 0,               /** Operation completed successfully. */
	DS_API_STATUS_NOT_INITIALIZED,      /** Driver is not initialized. */
	DS_API_STATUS_DEVICE_NOT_FOUND,     /** Device is not responding on the I2C bus. */
	DS_API_STATUS_WRITE_ERROR,          /** Write operation failed. */
	DS_API_STATUS_READ_ERROR,           /** Read operation failed. */
	DS_API_STATUS_INVALID_PARAMETERS    /** Invalid function parameters. */
} ds_api_status_t;

/**
 * @brief Time and date data structure.
 *
 * Stores date and time values in decimal format.
 */
typedef struct{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_of_week;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} ds_time_data_t;

/**
 * @brief Temperature measurement data.
 */
typedef struct{
	int16_t temperature_x100;
} ds_temperature_data_t;

ds_api_status_t ds_init				(I2C_HandleTypeDef *hi2c, const uint8_t device_address);
ds_api_status_t ds_read_time		(ds_time_data_t *const time_data);
ds_api_status_t ds_write_time		(const ds_time_data_t *const time_data, const bool is_24_hour_format);
ds_api_status_t ds_read_temperature	(ds_temperature_data_t *const temperature_data);

