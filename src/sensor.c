/**
 * @file sensor.c
 * @brief Core sensor implementation for the Industrial AI-Powered Edge Monitoring System
 * @author ELYES
 * @copyright Copyright (c) ELYES 2024-2025. All rights reserved.
 */

#include "../include/sensor.h"
#include <string.h>
#include <time.h>

// Error message strings
static const char* error_messages[] = {
    "No error",
    "Invalid parameter",
    "Initialization failed",
    "Read operation failed",
    "Value out of range",
    "Hardware error",
    "Memory allocation failed",
    "Communication error",
    "Calibration error"
};

// Sensor type strings
static const char* sensor_type_strings[] = {
    "Temperature",
    "Humidity",
    "Pressure",
    "Gas",
    "Vibration",
    "Current",
    "Voltage",
    "Power",
    "Flow",
    "Level",
    "Position",
    "Speed",
    "Acceleration",
    "Gyroscope",
    "Magnetic"
};

// Default initialization function
static bool default_init(void) {
    return true;
}

// Default cleanup function
static void default_cleanup(void) {
    // Nothing to do by default
}

bool sensor_init(Sensor* sensor, SensorType type, const char* id) {
    if (!sensor || !id) {
        if (sensor) {
            sensor->last_error = SENSOR_ERROR_INVALID_PARAM;
        }
        return false;
    }

    sensor->type = type;
    strncpy(sensor->id, id, sizeof(sensor->id) - 1);
    sensor->id[sizeof(sensor->id) - 1] = '\0';
    
    // Initialize new fields
    sensor->name[0] = '\0';
    sensor->location[0] = '\0';
    sensor->sample_count = 0;
    sensor->error_count = 0;
    
    // Set default functions
    sensor->initialize = default_init;
    sensor->cleanup = default_cleanup;
    
    // Initialize error state
    sensor->last_error = SENSOR_ERROR_NONE;
    
    if (!sensor->initialize()) {
        sensor->last_error = SENSOR_ERROR_INIT_FAILED;
        return false;
    }
    
    return true;
}

bool sensor_read_data(Sensor* sensor, SensorData* data) {
    if (!sensor || !data || !sensor->read) {
        if (sensor) {
            sensor->last_error = SENSOR_ERROR_INVALID_PARAM;
        }
        return false;
    }

    data->type = sensor->type;
    data->timestamp = (uint32_t)time(NULL);
    data->is_valid = false;
    data->error = SENSOR_ERROR_NONE;
    
    // Set default unit based on sensor type
    switch (sensor->type) {
        case SENSOR_TYPE_TEMPERATURE:
            strncpy(data->unit, "°C", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_HUMIDITY:
            strncpy(data->unit, "%", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_PRESSURE:
            strncpy(data->unit, "kPa", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_GAS:
            strncpy(data->unit, "ppm", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_VIBRATION:
            strncpy(data->unit, "g", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_CURRENT:
            strncpy(data->unit, "A", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_VOLTAGE:
            strncpy(data->unit, "V", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_POWER:
            strncpy(data->unit, "W", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_FLOW:
            strncpy(data->unit, "L/min", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_LEVEL:
            strncpy(data->unit, "m", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_POSITION:
            strncpy(data->unit, "mm", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_SPEED:
            strncpy(data->unit, "rpm", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_ACCELERATION:
            strncpy(data->unit, "m/s²", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_GYROSCOPE:
            strncpy(data->unit, "°/s", sizeof(data->unit) - 1);
            break;
        case SENSOR_TYPE_MAGNETIC:
            strncpy(data->unit, "µT", sizeof(data->unit) - 1);
            break;
        default:
            strncpy(data->unit, "", sizeof(data->unit) - 1);
            break;
    }
    data->unit[sizeof(data->unit) - 1] = '\0';

    // Increment sample count
    sensor->sample_count++;

    if (!sensor->read(data)) {
        sensor->last_error = data->error;
        sensor->error_count++;
        return false;
    }

    sensor->last_error = SENSOR_ERROR_NONE;
    return true;
}

void sensor_cleanup(Sensor* sensor) {
    if (sensor && sensor->cleanup) {
        sensor->cleanup();
    }
}

const char* sensor_error_to_string(SensorError error) {
    if (error >= sizeof(error_messages) / sizeof(error_messages[0])) {
        return "Unknown error";
    }
    return error_messages[error];
}

const char* sensor_type_to_string(SensorType type) {
    if (type >= sizeof(sensor_type_strings) / sizeof(sensor_type_strings[0])) {
        return "Unknown sensor type";
    }
    return sensor_type_strings[type];
}

void sensor_set_name(Sensor* sensor, const char* name) {
    if (sensor && name) {
        strncpy(sensor->name, name, sizeof(sensor->name) - 1);
        sensor->name[sizeof(sensor->name) - 1] = '\0';
    }
}

void sensor_set_location(Sensor* sensor, const char* location) {
    if (sensor && location) {
        strncpy(sensor->location, location, sizeof(sensor->location) - 1);
        sensor->location[sizeof(sensor->location) - 1] = '\0';
    }
}

void sensor_set_unit(Sensor* sensor, const char* unit) {
    // This function is a placeholder for future implementation
    // The unit is currently set in sensor_read_data based on sensor type
    (void)sensor;
    (void)unit;
} 