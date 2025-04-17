/**
 * @file temperature_sensor.h
 * @brief Temperature sensor implementation for the Industrial AI-Powered Edge Monitoring System
 * @author ELYES
 * @copyright Copyright (c) ELYES 2024-2025. All rights reserved.
 * 
 * @note This interface is thread-safe. All public functions can be called from multiple threads
 * simultaneously. The implementation uses appropriate synchronization mechanisms to ensure
 * data consistency.
 */

#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "sensor.h"

// Temperature sensor specific data
typedef struct {
    float temperature;    ///< Temperature in Celsius
    float humidity;       ///< Relative humidity in percentage (0-100)
    float dew_point;      ///< Calculated dew point in Celsius
    float heat_index;     ///< Calculated heat index in Celsius
} TemperatureSensorData;

// Temperature sensor configuration
typedef struct {
    float min_temp;           ///< Minimum valid temperature in Celsius
    float max_temp;           ///< Maximum valid temperature in Celsius
    float alert_threshold;    ///< Temperature threshold for alerts in Celsius
    float critical_threshold; ///< Temperature threshold for critical alerts in Celsius
    float calibration_offset; ///< Calibration offset in Celsius
    uint32_t sampling_rate_ms;///< Sampling rate in milliseconds
    bool enable_humidity;     ///< Enable humidity measurement
    bool enable_dew_point;    ///< Enable dew point calculation
    bool enable_heat_index;   ///< Enable heat index calculation
} TemperatureConfig;

// Temperature sensor statistics
typedef struct {
    float min_value;      ///< Minimum recorded temperature
    float max_value;      ///< Maximum recorded temperature
    float avg_value;      ///< Average temperature
    float std_deviation;  ///< Standard deviation of temperature
    uint32_t sample_count;///< Total number of samples
    uint32_t alert_count; ///< Number of alerts triggered
    uint32_t critical_count; ///< Number of critical alerts triggered
} TemperatureStats;

// Function prototypes
/**
 * @brief Initialize a temperature sensor
 * @param sensor Pointer to the sensor structure
 * @param id Unique identifier for the sensor
 * @param config Pointer to temperature sensor configuration
 * @return true if initialization successful, false otherwise
 * @note Thread-safe function
 */
bool temperature_sensor_init(Sensor* sensor, const char* id, TemperatureConfig* config);

/**
 * @brief Read temperature data from the sensor
 * @param sensor Pointer to the sensor structure
 * @param data Pointer to store the read data
 * @return true if read successful, false otherwise
 * @note Thread-safe function
 */
bool temperature_sensor_read(Sensor* sensor, SensorData* data);

/**
 * @brief Clean up resources used by the temperature sensor
 * @param sensor Pointer to the sensor structure
 * @note Thread-safe function
 */
void temperature_sensor_cleanup(Sensor* sensor);

/**
 * @brief Get temperature sensor statistics
 * @param sensor Pointer to the sensor structure
 * @param stats Pointer to store the statistics
 * @note Thread-safe function
 */
void temperature_sensor_get_stats(Sensor* sensor, TemperatureStats* stats);

/**
 * @brief Reset temperature sensor statistics
 * @param sensor Pointer to the sensor structure
 * @note Thread-safe function
 */
void temperature_sensor_reset_stats(Sensor* sensor);

/**
 * @brief Set temperature sensor configuration
 * @param sensor Pointer to the sensor structure
 * @param config Pointer to new configuration
 * @note Thread-safe function
 */
void temperature_sensor_set_config(Sensor* sensor, TemperatureConfig* config);

/**
 * @brief Get current temperature sensor configuration
 * @param sensor Pointer to the sensor structure
 * @param config Pointer to store the configuration
 * @note Thread-safe function
 */
void temperature_sensor_get_config(Sensor* sensor, TemperatureConfig* config);

/**
 * @brief Calculate dew point from temperature and humidity
 * @param temperature Temperature in Celsius
 * @param humidity Relative humidity in percentage (0-100)
 * @return Calculated dew point in Celsius
 */
float temperature_sensor_calculate_dew_point(float temperature, float humidity);

/**
 * @brief Calculate heat index from temperature and humidity
 * @param temperature Temperature in Celsius
 * @param humidity Relative humidity in percentage (0-100)
 * @return Calculated heat index in Celsius
 */
float temperature_sensor_calculate_heat_index(float temperature, float humidity);

#endif // TEMPERATURE_SENSOR_H 