# EdgeTrack API Documentation

## Overview

This document provides detailed information about the EdgeTrack API, including function descriptions, parameters, and usage examples.

## Sensor Management

### Base Sensor Interface

#### `Sensor* sensor_create(SensorConfig* config)`
Creates a new sensor instance with the specified configuration.

**Parameters:**
- `config`: Pointer to sensor configuration structure

**Returns:**
- Pointer to newly created sensor instance
- NULL on failure

**Example:**
```c
SensorConfig config = {
    .sensor_id = 1,
    .sampling_rate = 1000
};
Sensor* sensor = sensor_create(&config);
```

#### `void sensor_destroy(Sensor* sensor)`
Destroys a sensor instance and frees associated resources.

**Parameters:**
- `sensor`: Pointer to sensor instance to destroy

**Example:**
```c
sensor_destroy(sensor);
```

### Temperature Sensor

#### `TemperatureSensor* temperature_sensor_create(TemperatureSensorConfig* config)`
Creates a new temperature sensor instance.

**Parameters:**
- `config`: Pointer to temperature sensor configuration

**Returns:**
- Pointer to newly created temperature sensor
- NULL on failure

**Example:**
```c
TemperatureSensorConfig config = {
    .sensor_id = 1,
    .sampling_rate = 1000,
    .min_value = -40.0f,
    .max_value = 125.0f
};
TemperatureSensor* sensor = temperature_sensor_create(&config);
```

#### `int temperature_sensor_start(TemperatureSensor* sensor)`
Starts the temperature sensor monitoring.

**Parameters:**
- `sensor`: Pointer to temperature sensor instance

**Returns:**
- 0 on success
- Negative value on error

**Example:**
```c
if (temperature_sensor_start(sensor) != 0) {
    // Handle error
}
```

## Logging System

### Logger Configuration

#### `int logger_init(LoggerConfig* config)`
Initializes the logging system.

**Parameters:**
- `config`: Pointer to logger configuration

**Returns:**
- 0 on success
- Negative value on error

**Example:**
```c
LoggerConfig config = {
    .log_file_path = "logs/edgetrack.log",
    .min_log_level = LOG_LEVEL_INFO,
    .max_file_size = 10 * 1024 * 1024,
    .max_files = 5,
    .log_to_console = true,
    .log_to_file = true,
    .include_timestamp = true
};
logger_init(&config);
```

### Logging Functions

#### `void logger_log(LogLevel level, const char* format, ...)`
Logs a message with the specified level.

**Parameters:**
- `level`: Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- `format`: printf-style format string
- `...`: Variable arguments for format string

**Example:**
```c
logger_log(LOG_LEVEL_INFO, "System started");
logger_log(LOG_LEVEL_ERROR, "Failed to read sensor: %s", error_message);
```

#### `void logger_log_sensor_data(const SensorData* data)`
Logs sensor data.

**Parameters:**
- `data`: Pointer to sensor data structure

**Example:**
```c
SensorData data = {
    .sensor_id = 1,
    .timestamp = time(NULL),
    .value = 23.5,
    .unit = "°C"
};
logger_log_sensor_data(&data);
```

## Error Handling

### Error Codes

- `SENSOR_ERROR_NONE`: No error
- `SENSOR_ERROR_INIT`: Initialization error
- `SENSOR_ERROR_READ`: Read error
- `SENSOR_ERROR_WRITE`: Write error
- `SENSOR_ERROR_INVALID_PARAM`: Invalid parameter
- `SENSOR_ERROR_TIMEOUT`: Operation timeout
- `SENSOR_ERROR_NOT_IMPLEMENTED`: Feature not implemented

### Error Recovery

The system implements automatic error recovery mechanisms:
1. Retry operations on temporary failures
2. Log errors for debugging
3. Maintain system stability during errors
4. Provide error status through return values

## Thread Safety

All public API functions are thread-safe and can be called from multiple threads simultaneously. The implementation uses appropriate synchronization mechanisms to ensure data consistency.

## Memory Management

The API follows these memory management rules:
1. All allocated resources must be freed using corresponding destroy/cleanup functions
2. No memory leaks in error conditions
3. Proper cleanup in all exit paths
4. Clear ownership of allocated resources

## Best Practices

1. Always check return values from API functions
2. Clean up resources using appropriate destroy/cleanup functions
3. Use appropriate log levels for different types of messages
4. Handle errors gracefully
5. Follow the initialization -> use -> cleanup pattern 