/**
 * @file main.c
 * @brief Main program for the Industrial AI-Powered Edge Monitoring System
 * @author ELYES
 * @copyright Copyright (c) ELYES 2024-2025. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include "../include/sensor.h"
#include "../include/temperature_sensor.h"

#define SAMPLE_INTERVAL_SECONDS 1
#define LOG_FILE "sensor_data.csv"
#define MAX_SAMPLES 1000

// Global flag for graceful shutdown
static volatile int running = 1;

// Signal handler for graceful shutdown
static void handle_signal(int signum) {
    (void)signum;  // Unused parameter
    running = 0;
}

// Function to log sensor data to file
static void log_sensor_data(const Sensor* sensor, const SensorData* data, FILE* log_file) {
    if (!sensor || !data || !log_file) {
        return;
    }
    
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    fprintf(log_file, "%s,%s,%s,%.2f,%s,%s,%s\n",
            timestamp,
            sensor->id,
            sensor_type_to_string(sensor->type),
            data->value,
            data->unit,
            data->is_valid ? "Valid" : "Invalid",
            data->error == SENSOR_ERROR_NONE ? "No Error" : sensor_error_to_string(data->error));
    
    fflush(log_file);
}

// Function to print sensor statistics
static void print_sensor_stats(const Sensor* sensor) {
    if (!sensor) {
        return;
    }
    
    TemperatureStats stats;
    temperature_sensor_get_stats(sensor, &stats);
    
    printf("\nSensor Statistics:\n");
    printf("  Samples: %u\n", stats.sample_count);
    printf("  Min Value: %.2f°C\n", stats.min_value);
    printf("  Max Value: %.2f°C\n", stats.max_value);
    printf("  Average: %.2f°C\n", stats.avg_value);
    printf("  Alerts: %u\n", stats.alert_count);
    printf("  Critical: %u\n", stats.critical_count);
    printf("  Error Rate: %.2f%%\n", 
           stats.sample_count > 0 ? (float)stats.critical_count / stats.sample_count * 100.0f : 0.0f);
}

int main() {
    printf("Industrial AI-Powered Edge Monitoring System\n");
    printf("Copyright (c) ELYES 2024-2025. All rights reserved.\n\n");
    
    // Set up signal handling
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // Open log file
    FILE* log_file = fopen(LOG_FILE, "w");
    if (!log_file) {
        printf("Error: Could not open log file %s\n", LOG_FILE);
        return 1;
    }
    
    // Write CSV header
    fprintf(log_file, "Timestamp,Sensor ID,Sensor Type,Value,Unit,Valid,Error\n");
    fflush(log_file);

    // Initialize temperature sensor configuration
    TemperatureConfig temp_config = {
        .min_temp = 0.0f,
        .max_temp = 50.0f,
        .alert_threshold = 40.0f,
        .critical_threshold = 45.0f,
        .calibration_offset = 0.0f,
        .sampling_rate_ms = 1000,
        .enable_humidity = true,
        .enable_dew_point = true,
        .enable_heat_index = true
    };

    // Create and initialize temperature sensor
    Sensor temp_sensor;
    if (!temperature_sensor_init(&temp_sensor, "TEMP001", &temp_config)) {
        printf("Failed to initialize temperature sensor: %s\n", 
               sensor_error_to_string(temp_sensor.last_error));
        fclose(log_file);
        return 1;
    }

    printf("Temperature sensor initialized successfully\n");
    printf("Starting monitoring loop... (Press Ctrl+C to stop)\n\n");
    printf("Configuration:\n");
    printf("  Min Temperature: %.1f°C\n", temp_config.min_temp);
    printf("  Max Temperature: %.1f°C\n", temp_config.max_temp);
    printf("  Alert Threshold: %.1f°C\n", temp_config.alert_threshold);
    printf("  Critical Threshold: %.1f°C\n", temp_config.critical_threshold);
    printf("  Sampling Rate: %u ms\n", temp_config.sampling_rate_ms);
    printf("  Humidity Enabled: %s\n", temp_config.enable_humidity ? "Yes" : "No");
    printf("  Dew Point Enabled: %s\n", temp_config.enable_dew_point ? "Yes" : "No");
    printf("  Heat Index Enabled: %s\n\n", temp_config.enable_heat_index ? "Yes" : "No");

    // Main monitoring loop
    uint32_t sample_count = 0;
    while (running) {
        SensorData sensor_data;
        
        // Read sensor data
        if (temperature_sensor_read(&temp_sensor, &sensor_data)) {
            printf("Temperature: %.2f%s (Valid: %s)", 
                   sensor_data.value,
                   sensor_data.unit,
                   sensor_data.is_valid ? "Yes" : "No");
            
            if (sensor_data.error != SENSOR_ERROR_NONE) {
                printf(" [WARNING: %s]", sensor_error_to_string(sensor_data.error));
            }
            printf("\n");
            
            // Log data to file
            log_sensor_data(&temp_sensor, &sensor_data, log_file);
            
            // Print statistics every 100 samples
            sample_count++;
            if (sample_count % 100 == 0) {
                print_sensor_stats(&temp_sensor);
            }
        } else {
            printf("Error reading temperature sensor: %s\n", 
                   sensor_error_to_string(temp_sensor.last_error));
        }

        // Wait for next sample
        sleep(SAMPLE_INTERVAL_SECONDS);
    }

    printf("\nShutting down...\n");
    
    // Print final statistics
    print_sensor_stats(&temp_sensor);
    
    // Cleanup
    temperature_sensor_cleanup(&temp_sensor);
    fclose(log_file);
    
    printf("Done by ELYES\n");
    return 0;
} 