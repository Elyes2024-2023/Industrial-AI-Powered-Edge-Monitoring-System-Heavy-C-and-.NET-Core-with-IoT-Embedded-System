#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/sensor.h"
#include "../include/temperature_sensor.h"
#include "../include/logger.h"

// Test configuration
static const char* TEST_LOG_FILE = "test.log";
static const int TEST_SENSOR_ID = 1;
static const int TEST_SAMPLING_RATE = 1000;
static const float TEST_MIN_VALUE = -40.0f;
static const float TEST_MAX_VALUE = 125.0f;

// Test logger initialization
static int test_logger_init(void) {
    LoggerConfig config = {
        .log_file_path = TEST_LOG_FILE,
        .min_log_level = LOG_LEVEL_DEBUG,
        .max_file_size = 1024 * 1024,  // 1MB
        .max_files = 3,
        .log_to_console = true,
        .log_to_file = true,
        .include_timestamp = true
    };

    int result = logger_init(&config);
    assert(result == 0 && "Logger initialization failed");
    return result;
}

// Test sensor creation
static int test_sensor_create(void) {
    SensorConfig config = {
        .sensor_id = TEST_SENSOR_ID,
        .sampling_rate = TEST_SAMPLING_RATE
    };

    Sensor* sensor = sensor_create(&config);
    assert(sensor != NULL && "Sensor creation failed");

    sensor_destroy(sensor);
    return 0;
}

// Test temperature sensor creation
static int test_temperature_sensor_create(void) {
    TemperatureSensorConfig config = {
        .sensor_id = TEST_SENSOR_ID,
        .sampling_rate = TEST_SAMPLING_RATE,
        .min_value = TEST_MIN_VALUE,
        .max_value = TEST_MAX_VALUE
    };

    TemperatureSensor* sensor = temperature_sensor_create(&config);
    assert(sensor != NULL && "Temperature sensor creation failed");

    temperature_sensor_destroy(sensor);
    return 0;
}

// Test temperature sensor start/stop
static int test_temperature_sensor_control(void) {
    TemperatureSensorConfig config = {
        .sensor_id = TEST_SENSOR_ID,
        .sampling_rate = TEST_SAMPLING_RATE,
        .min_value = TEST_MIN_VALUE,
        .max_value = TEST_MAX_VALUE
    };

    TemperatureSensor* sensor = temperature_sensor_create(&config);
    assert(sensor != NULL && "Temperature sensor creation failed");

    int result = temperature_sensor_start(sensor);
    assert(result == 0 && "Temperature sensor start failed");

    // Wait for a few samples
    Sleep(3000);  // Windows-specific, use sleep(3) on Unix

    result = temperature_sensor_stop(sensor);
    assert(result == 0 && "Temperature sensor stop failed");

    temperature_sensor_destroy(sensor);
    return 0;
}

// Test error handling
static int test_error_handling(void) {
    // Test invalid configuration
    TemperatureSensorConfig config = {
        .sensor_id = TEST_SENSOR_ID,
        .sampling_rate = 0,  // Invalid sampling rate
        .min_value = TEST_MAX_VALUE,
        .max_value = TEST_MIN_VALUE  // Invalid range
    };

    TemperatureSensor* sensor = temperature_sensor_create(&config);
    assert(sensor == NULL && "Should fail with invalid configuration");

    return 0;
}

// Main test function
int main(void) {
    printf("Running EdgeTrack tests...\n");

    // Initialize logger
    if (test_logger_init() != 0) {
        printf("Logger initialization test failed\n");
        return 1;
    }
    printf("Logger initialization test passed\n");

    // Test sensor creation
    if (test_sensor_create() != 0) {
        printf("Sensor creation test failed\n");
        return 1;
    }
    printf("Sensor creation test passed\n");

    // Test temperature sensor creation
    if (test_temperature_sensor_create() != 0) {
        printf("Temperature sensor creation test failed\n");
        return 1;
    }
    printf("Temperature sensor creation test passed\n");

    // Test temperature sensor control
    if (test_temperature_sensor_control() != 0) {
        printf("Temperature sensor control test failed\n");
        return 1;
    }
    printf("Temperature sensor control test passed\n");

    // Test error handling
    if (test_error_handling() != 0) {
        printf("Error handling test failed\n");
        return 1;
    }
    printf("Error handling test passed\n");

    // Cleanup
    logger_cleanup();

    printf("All tests passed successfully!\n");
    return 0;
} 