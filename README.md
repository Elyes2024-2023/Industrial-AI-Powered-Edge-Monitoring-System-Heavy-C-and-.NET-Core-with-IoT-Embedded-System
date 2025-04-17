# Industrial AI-Powered Edge Monitoring System

A comprehensive IoT monitoring system for industrial equipment with real-time data collection, analysis, and alerting capabilities, implemented in C.

## Project Overview

This system provides:
- Real-time sensor data collection from industrial equipment
- Configurable logging system with file rotation
- Temperature sensor simulation and management
- Thread-safe operations
- Error handling and recovery mechanisms

## Architecture

The system consists of:
- Core sensor management system
- Temperature sensor implementation
- Configurable logging system
- Error handling and recovery
- File-based data storage

### Components

1. **Sensor Management**
   - Base sensor interface
   - Configurable sensor parameters
   - Error handling and recovery
   - Thread-safe operations

2. **Temperature Sensor**
   - Temperature data collection
   - Configurable sampling rate
   - Data validation and filtering
   - Error detection and reporting

3. **Logging System**
   - Multiple log levels (DEBUG, INFO, WARNING, ERROR, CRITICAL)
   - File and console output
   - Log rotation
   - Timestamp support
   - Thread-safe logging

## Getting Started

### Prerequisites
- C compiler (GCC or MSVC)
- Make build system
- Standard C libraries

### Building the Project

```bash
# Clone the repository
git clone https://github.com/Elyes2024-2023/Industrial-AI-Powered-Edge-Monitoring-System-Heavy-C-and-.NET-Core-with-IoT-Embedded-System.git
cd Industrial-AI-Powered-Edge-Monitoring-System-Heavy-C-and-.NET-Core-with-IoT-Embedded-System

# Build the project
make

# Run the program
make run

# Build with debug information
make debug

# Build with profiling
make profile

# Build static binary
make static
```

### Project Structure

```
.
├── include/           # Header files
│   ├── sensor.h
│   ├── temperature_sensor.h
│   └── logger.h
├── src/              # Source files
│   ├── main.c
│   ├── sensor.c
│   ├── temperature_sensor.c
│   └── logger.c
├── docs/             # Documentation
├── tests/            # Test files
├── lib/              # Library files
├── Makefile          # Build configuration
└── README.md         # This file
```

## Usage

### Basic Usage

```c
#include "sensor.h"
#include "temperature_sensor.h"
#include "logger.h"

int main() {
    // Initialize logger
    LoggerConfig log_config = {
        .log_file_path = "logs/edgetrack.log",
        .min_log_level = LOG_LEVEL_INFO,
        .max_file_size = 10 * 1024 * 1024,  // 10MB
        .max_files = 5,
        .log_to_console = true,
        .log_to_file = true,
        .include_timestamp = true
    };
    logger_init(&log_config);

    // Initialize temperature sensor
    TemperatureSensorConfig temp_config = {
        .sensor_id = 1,
        .sampling_rate = 1000,  // 1 second
        .min_value = -40.0f,
        .max_value = 125.0f
    };
    TemperatureSensor* sensor = temperature_sensor_create(&temp_config);

    // Start monitoring
    temperature_sensor_start(sensor);

    // Clean up
    temperature_sensor_destroy(sensor);
    logger_cleanup();

    return 0;
}
```

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**ELYES** - *Initial work*

---

*Industrial AI-Powered Edge Monitoring System - Done by ELYES* 