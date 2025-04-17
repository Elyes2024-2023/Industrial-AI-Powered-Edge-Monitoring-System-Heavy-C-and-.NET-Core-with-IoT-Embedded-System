/**
 * @file logger.h
 * @brief Data logging functionality for the Industrial AI-Powered Edge Monitoring System
 * @author ELYES
 * @copyright Copyright (c) ELYES 2024-2025. All rights reserved.
 * 
 * @note This interface is thread-safe. All public functions can be called from multiple threads
 * simultaneously. The implementation uses appropriate synchronization mechanisms to ensure
 * data consistency.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "sensor.h"

// Log levels
typedef enum {
    LOG_LEVEL_DEBUG = 0,    ///< Debug information
    LOG_LEVEL_INFO,         ///< General information
    LOG_LEVEL_WARNING,      ///< Warning messages
    LOG_LEVEL_ERROR,        ///< Error messages
    LOG_LEVEL_CRITICAL      ///< Critical error messages
} LogLevel;

// Log entry structure
typedef struct {
    uint32_t timestamp;     ///< Unix timestamp in seconds
    LogLevel level;         ///< Log level
    char message[256];      ///< Log message
    char sensor_id[32];     ///< Sensor identifier
    char sensor_type[32];   ///< Sensor type
    float value;           ///< Sensor value
    char unit[16];         ///< Unit of measurement
    bool is_valid;         ///< Whether the data is valid
    char error[64];        ///< Error message if any
} LogEntry;

// Logger configuration
typedef struct {
    char log_file[128];     ///< Path to log file
    LogLevel min_level;     ///< Minimum log level to record
    bool log_to_console;    ///< Whether to log to console
    bool log_to_file;       ///< Whether to log to file
    bool log_timestamp;     ///< Whether to include timestamps
    bool log_sensor_data;   ///< Whether to log sensor data
    uint32_t max_file_size_kb; ///< Maximum log file size in kilobytes
    uint32_t max_files;     ///< Maximum number of log files to keep
} LoggerConfig;

// Function prototypes
/**
 * @brief Initialize the logger with the specified configuration
 * @param config Pointer to logger configuration
 * @return true if initialization successful, false otherwise
 * @note Thread-safe function
 */
bool logger_init(const LoggerConfig* config);

/**
 * @brief Clean up logger resources
 * @note Thread-safe function
 */
void logger_cleanup(void);

/**
 * @brief Log a message with the specified level
 * @param level Log level
 * @param message Message to log
 * @return true if logging successful, false otherwise
 * @note Thread-safe function
 */
bool logger_log(LogLevel level, const char* message);

/**
 * @brief Log sensor data
 * @param sensor Pointer to the sensor
 * @param data Pointer to the sensor data
 * @param level Log level
 * @return true if logging successful, false otherwise
 * @note Thread-safe function
 */
bool logger_log_sensor_data(const Sensor* sensor, const SensorData* data, LogLevel level);

/**
 * @brief Convert log level to string
 * @param level Log level to convert
 * @return String representation of the log level
 */
const char* logger_level_to_string(LogLevel level);

/**
 * @brief Convert string to log level
 * @param level_str String representation of log level
 * @return Corresponding log level
 */
LogLevel logger_string_to_level(const char* level_str);

/**
 * @brief Set logger configuration
 * @param config Pointer to new configuration
 * @note Thread-safe function
 */
void logger_set_config(const LoggerConfig* config);

/**
 * @brief Get current logger configuration
 * @param config Pointer to store the configuration
 * @note Thread-safe function
 */
void logger_get_config(LoggerConfig* config);

/**
 * @brief Rotate log file if size limit is reached
 * @note Thread-safe function
 */
void logger_rotate_log_file(void);

#endif // LOGGER_H 