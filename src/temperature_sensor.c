/**
 * @file temperature_sensor.c
 * @brief Temperature sensor implementation for the Industrial AI-Powered Edge Monitoring System
 * @author ELYES
 * @copyright Copyright (c) ELYES 2024-2025. All rights reserved.
 */

#include "../include/temperature_sensor.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Private data structure
typedef struct {
    TemperatureConfig config;
    TemperatureSensorData last_reading;
    TemperatureStats stats;
    uint32_t last_sample_time;
} TemperatureSensorPrivate;

// Forward declarations of private functions
static bool temperature_init(void);
static bool temperature_read(SensorData* data);
static void temperature_cleanup(void);
static void update_stats(TemperatureSensorPrivate* private, float value);
static float calculate_dew_point(float temperature, float humidity);
static float calculate_heat_index(float temperature, float humidity);

// Private data instance
static TemperatureSensorPrivate* private_data = NULL;

bool temperature_sensor_init(Sensor* sensor, const char* id, TemperatureConfig* config) {
    if (!sensor || !id || !config) {
        if (sensor) {
            sensor->last_error = SENSOR_ERROR_INVALID_PARAM;
        }
        return false;
    }

    // Initialize base sensor
    if (!sensor_init(sensor, SENSOR_TYPE_TEMPERATURE, id)) {
        return false;
    }

    // Allocate and initialize private data
    private_data = (TemperatureSensorPrivate*)malloc(sizeof(TemperatureSensorPrivate));
    if (!private_data) {
        sensor->last_error = SENSOR_ERROR_MEMORY;
        return false;
    }

    // Copy configuration
    memcpy(&private_data->config, config, sizeof(TemperatureConfig));
    
    // Initialize last reading
    private_data->last_reading.temperature = 0.0f;
    private_data->last_reading.humidity = 0.0f;
    private_data->last_reading.dew_point = 0.0f;
    private_data->last_reading.heat_index = 0.0f;
    
    // Initialize statistics
    private_data->stats.min_value = FLT_MAX;
    private_data->stats.max_value = -FLT_MAX;
    private_data->stats.avg_value = 0.0f;
    private_data->stats.std_deviation = 0.0f;
    private_data->stats.sample_count = 0;
    private_data->stats.alert_count = 0;
    private_data->stats.critical_count = 0;
    
    // Initialize last sample time
    private_data->last_sample_time = 0;

    // Set sensor interface functions
    sensor->initialize = temperature_init;
    sensor->read = temperature_read;
    sensor->cleanup = temperature_cleanup;
    
    // Set sensor name and location
    sensor_set_name(sensor, "Temperature Sensor");
    sensor_set_location(sensor, "Factory Floor");

    return true;
}

static bool temperature_init(void) {
    // Here you would initialize the actual hardware
    // For now, we'll just return success
    return true;
}

static bool temperature_read(SensorData* data) {
    if (!data || !private_data) {
        if (data) {
            data->error = SENSOR_ERROR_INVALID_PARAM;
        }
        return false;
    }

    // Check if we need to wait for the next sample
    uint32_t current_time = (uint32_t)time(NULL) * 1000; // Convert to milliseconds
    if (current_time - private_data->last_sample_time < private_data->config.sampling_rate_ms) {
        // Return the last reading
        data->value = private_data->last_reading.temperature;
        data->is_valid = true;
        data->error = SENSOR_ERROR_NONE;
        return true;
    }

    // Here you would read from the actual hardware
    // For demonstration, we'll simulate a reading with some variation
    float base_temp = 25.0f;
    float variation = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // -1 to 1
    private_data->last_reading.temperature = base_temp + variation + private_data->config.calibration_offset;
    
    if (private_data->config.enable_humidity) {
        private_data->last_reading.humidity = 45.0f + ((float)rand() / RAND_MAX) * 10.0f; // 45-55%
    } else {
        private_data->last_reading.humidity = 0.0f;
    }
    
    // Calculate derived values
    if (private_data->config.enable_dew_point && private_data->last_reading.humidity > 0.0f) {
        private_data->last_reading.dew_point = calculate_dew_point(
            private_data->last_reading.temperature, 
            private_data->last_reading.humidity
        );
    }
    
    if (private_data->config.enable_heat_index && private_data->last_reading.humidity > 0.0f) {
        private_data->last_reading.heat_index = calculate_heat_index(
            private_data->last_reading.temperature, 
            private_data->last_reading.humidity
        );
    }

    // Update the sensor data
    data->value = private_data->last_reading.temperature;
    data->is_valid = true;
    data->error = SENSOR_ERROR_NONE;
    
    // Update statistics
    update_stats(private_data, data->value);
    
    // Update last sample time
    private_data->last_sample_time = current_time;

    // Check if temperature is within bounds
    if (data->value < private_data->config.min_temp || 
        data->value > private_data->config.max_temp) {
        data->error = SENSOR_ERROR_OUT_OF_RANGE;
        return false;
    }

    // Check if temperature exceeds alert threshold
    if (data->value > private_data->config.alert_threshold) {
        // Still return true but set error to indicate warning
        data->error = SENSOR_ERROR_OUT_OF_RANGE;
        private_data->stats.alert_count++;
    }
    
    // Check if temperature exceeds critical threshold
    if (data->value > private_data->config.critical_threshold) {
        data->error = SENSOR_ERROR_OUT_OF_RANGE;
        private_data->stats.critical_count++;
    }

    return true;
}

static void temperature_cleanup(void) {
    if (private_data) {
        free(private_data);
        private_data = NULL;
    }
}

void temperature_sensor_cleanup(Sensor* sensor) {
    if (sensor) {
        sensor_cleanup(sensor);
    }
}

void temperature_sensor_get_stats(Sensor* sensor, TemperatureStats* stats) {
    if (!sensor || !stats || !private_data) {
        return;
    }
    
    memcpy(stats, &private_data->stats, sizeof(TemperatureStats));
}

void temperature_sensor_reset_stats(Sensor* sensor) {
    if (!sensor || !private_data) {
        return;
    }
    
    private_data->stats.min_value = FLT_MAX;
    private_data->stats.max_value = -FLT_MAX;
    private_data->stats.avg_value = 0.0f;
    private_data->stats.std_deviation = 0.0f;
    private_data->stats.sample_count = 0;
    private_data->stats.alert_count = 0;
    private_data->stats.critical_count = 0;
}

void temperature_sensor_set_config(Sensor* sensor, TemperatureConfig* config) {
    if (!sensor || !config || !private_data) {
        return;
    }
    
    memcpy(&private_data->config, config, sizeof(TemperatureConfig));
}

void temperature_sensor_get_config(Sensor* sensor, TemperatureConfig* config) {
    if (!sensor || !config || !private_data) {
        return;
    }
    
    memcpy(config, &private_data->config, sizeof(TemperatureConfig));
}

float temperature_sensor_calculate_dew_point(float temperature, float humidity) {
    return calculate_dew_point(temperature, humidity);
}

float temperature_sensor_calculate_heat_index(float temperature, float humidity) {
    return calculate_heat_index(temperature, humidity);
}

// Private helper functions
static void update_stats(TemperatureSensorPrivate* private, float value) {
    if (!private) {
        return;
    }
    
    // Update min/max
    if (value < private->stats.min_value) {
        private->stats.min_value = value;
    }
    if (value > private->stats.max_value) {
        private->stats.max_value = value;
    }
    
    // Update average
    private->stats.avg_value = (private->stats.avg_value * private->stats.sample_count + value) / 
                              (private->stats.sample_count + 1);
    
    // Update sample count
    private->stats.sample_count++;
    
    // Note: Standard deviation calculation would require storing all values
    // For simplicity, we're not implementing it here
}

static float calculate_dew_point(float temperature, float humidity) {
    // Simple approximation formula for dew point
    // Td = T - ((100 - RH) / 5)
    // where Td is dew point in °C, T is temperature in °C, and RH is relative humidity in %
    return temperature - ((100.0f - humidity) / 5.0f);
}

static float calculate_heat_index(float temperature, float humidity) {
    // Simple approximation formula for heat index
    // HI = 0.5 * (T + 61.0 + ((T-68.0)*1.2) + (RH*0.094))
    // where HI is heat index in °F, T is temperature in °F, and RH is relative humidity in %
    
    // Convert temperature from °C to °F
    float temp_f = temperature * 9.0f / 5.0f + 32.0f;
    
    // Calculate heat index in °F
    float hi_f = 0.5f * (temp_f + 61.0f + ((temp_f - 68.0f) * 1.2f) + (humidity * 0.094f));
    
    // Convert back to °C
    return (hi_f - 32.0f) * 5.0f / 9.0f;
} 