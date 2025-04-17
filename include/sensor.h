/**
 * @file sensor.h
 * @brief Core sensor interface for the Industrial AI-Powered Edge Monitoring System
 * @author ELYES
 * @copyright Copyright (c) ELYES 2024-2025. All rights reserved.
 * 
 * @note This interface is thread-safe. All public functions can be called from multiple threads
 * simultaneously. The implementation uses appropriate synchronization mechanisms to ensure
 * data consistency.
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Error codes
typedef enum {
    SENSOR_ERROR_NONE = 0,        ///< No error occurred
    SENSOR_ERROR_INVALID_PARAM,   ///< Invalid parameter provided
    SENSOR_ERROR_INIT_FAILED,     ///< Initialization failed
    SENSOR_ERROR_READ_FAILED,     ///< Reading sensor data failed
    SENSOR_ERROR_OUT_OF_RANGE,    ///< Sensor value out of valid range
    SENSOR_ERROR_HARDWARE,        ///< Hardware-related error
    SENSOR_ERROR_MEMORY,          ///< Memory allocation failed
    SENSOR_ERROR_COMMUNICATION,   ///< Communication with sensor failed
    SENSOR_ERROR_CALIBRATION      ///< Sensor calibration failed
} SensorError;

// Sensor types
typedef enum {
    SENSOR_TYPE_TEMPERATURE,
    SENSOR_TYPE_HUMIDITY,
    SENSOR_TYPE_PRESSURE,
    SENSOR_TYPE_GAS,
    SENSOR_TYPE_VIBRATION,
    SENSOR_TYPE_CURRENT,
    SENSOR_TYPE_VOLTAGE,
    SENSOR_TYPE_POWER,
    SENSOR_TYPE_FLOW,
    SENSOR_TYPE_LEVEL,
    SENSOR_TYPE_POSITION,
    SENSOR_TYPE_SPEED,
    SENSOR_TYPE_ACCELERATION,
    SENSOR_TYPE_GYROSCOPE,
    SENSOR_TYPE_MAGNETIC
} SensorType;

// Sensor data structure
typedef struct {
    SensorType type;
    float value;
    uint32_t timestamp;
    bool is_valid;
    SensorError error;  // Added error field
    char unit[16];      // Added unit field (e.g., "°C", "kPa", etc.)
} SensorData;

// Sensor interface
typedef struct {
    SensorType type;
    char id[32];
    char name[64];      // Added name field
    char location[64];  // Added location field
    bool (*initialize)(void);
    bool (*read)(SensorData* data);
    void (*cleanup)(void);
    SensorError last_error;  // Added last error field
    uint32_t sample_count;   // Added sample count field
    uint32_t error_count;    // Added error count field
} Sensor;

// Function prototypes
/**
 * @brief Initialize a sensor with the specified type and ID
 * @param sensor Pointer to the sensor structure to initialize
 * @param type Type of the sensor
 * @param id Unique identifier for the sensor
 * @return true if initialization successful, false otherwise
 * @note Thread-safe function
 */
bool sensor_init(Sensor* sensor, SensorType type, const char* id);

/**
 * @brief Read data from the sensor
 * @param sensor Pointer to the sensor structure
 * @param data Pointer to store the read data
 * @return true if read successful, false otherwise
 * @note Thread-safe function
 */
bool sensor_read_data(Sensor* sensor, SensorData* data);

/**
 * @brief Clean up resources used by the sensor
 * @param sensor Pointer to the sensor structure
 * @note Thread-safe function
 */
void sensor_cleanup(Sensor* sensor);

/**
 * @brief Convert sensor error code to string
 * @param error Error code to convert
 * @return String representation of the error
 */
const char* sensor_error_to_string(SensorError error);

/**
 * @brief Convert sensor type to string
 * @param type Sensor type to convert
 * @return String representation of the sensor type
 */
const char* sensor_type_to_string(SensorType type);

/**
 * @brief Set the name of the sensor
 * @param sensor Pointer to the sensor structure
 * @param name New name for the sensor
 * @note Thread-safe function
 */
void sensor_set_name(Sensor* sensor, const char* name);

/**
 * @brief Set the location of the sensor
 * @param sensor Pointer to the sensor structure
 * @param location New location for the sensor
 * @note Thread-safe function
 */
void sensor_set_location(Sensor* sensor, const char* location);

/**
 * @brief Set the unit of measurement for the sensor
 * @param sensor Pointer to the sensor structure
 * @param unit New unit for the sensor (e.g., "°C", "kPa")
 * @note Thread-safe function
 */
void sensor_set_unit(Sensor* sensor, const char* unit);

#endif // SENSOR_H 