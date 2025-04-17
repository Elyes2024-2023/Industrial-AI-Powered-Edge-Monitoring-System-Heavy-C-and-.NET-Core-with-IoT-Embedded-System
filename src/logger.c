/**
 * @file logger.c
 * @brief Data logging implementation for the Industrial AI-Powered Edge Monitoring System
 * @author ELYES
 * @copyright Copyright (c) ELYES 2024-2025. All rights reserved.
 */

#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>

// Private data structure
typedef struct {
    LoggerConfig config;
    FILE* log_file;
    uint32_t current_file_size;
    uint32_t file_count;
} LoggerPrivate;

// Private data instance
static LoggerPrivate* private_data = NULL;

// Log level strings
static const char* level_strings[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "CRITICAL"
};

// Default configuration
static const LoggerConfig default_config = {
    .log_file = "logs/edgetrack.log",
    .min_level = LOG_LEVEL_INFO,
    .log_to_console = true,
    .log_to_file = true,
    .log_timestamp = true,
    .log_sensor_data = true,
    .max_file_size_kb = 1024,
    .max_files = 5
};

bool logger_init(const LoggerConfig* config) {
    // Allocate private data
    private_data = (LoggerPrivate*)malloc(sizeof(LoggerPrivate));
    if (!private_data) {
        return false;
    }
    
    // Initialize with default configuration
    memcpy(&private_data->config, &default_config, sizeof(LoggerConfig));
    
    // Override with provided configuration if any
    if (config) {
        memcpy(&private_data->config, config, sizeof(LoggerConfig));
    }
    
    // Initialize other fields
    private_data->log_file = NULL;
    private_data->current_file_size = 0;
    private_data->file_count = 0;
    
    // Open log file if file logging is enabled
    if (private_data->config.log_to_file) {
        // Create directory if it doesn't exist
        char dir[128];
        strncpy(dir, private_data->config.log_file, sizeof(dir) - 1);
        dir[sizeof(dir) - 1] = '\0';
        
        char* last_slash = strrchr(dir, '/');
        if (last_slash) {
            *last_slash = '\0';
            
            // Create directory
            #ifdef _WIN32
            system("mkdir \"%s\" 2>nul", dir);
            #else
            char cmd[256];
            snprintf(cmd, sizeof(cmd), "mkdir -p \"%s\"", dir);
            system(cmd);
            #endif
        }
        
        // Open log file
        private_data->log_file = fopen(private_data->config.log_file, "a");
        if (!private_data->log_file) {
            free(private_data);
            private_data = NULL;
            return false;
        }
        
        // Get current file size
        struct stat st;
        if (stat(private_data->config.log_file, &st) == 0) {
            private_data->current_file_size = (uint32_t)st.st_size / 1024; // Convert to KB
        }
    }
    
    // Log initialization
    logger_log(LOG_LEVEL_INFO, "Logger initialized");
    
    return true;
}

void logger_cleanup(void) {
    if (private_data) {
        if (private_data->log_file) {
            logger_log(LOG_LEVEL_INFO, "Logger shutting down");
            fclose(private_data->log_file);
            private_data->log_file = NULL;
        }
        
        free(private_data);
        private_data = NULL;
    }
}

bool logger_log(LogLevel level, const char* message) {
    if (!private_data || !message || level < private_data->config.min_level) {
        return false;
    }
    
    // Create log entry
    LogEntry entry;
    entry.timestamp = (uint32_t)time(NULL);
    entry.level = level;
    strncpy(entry.message, message, sizeof(entry.message) - 1);
    entry.message[sizeof(entry.message) - 1] = '\0';
    entry.sensor_id[0] = '\0';
    entry.sensor_type[0] = '\0';
    entry.value = 0.0f;
    entry.unit[0] = '\0';
    entry.is_valid = false;
    entry.error[0] = '\0';
    
    // Format log message
    char formatted_message[512];
    
    if (private_data->config.log_timestamp) {
        time_t now = time(NULL);
        struct tm* timeinfo = localtime(&now);
        char timestamp[32];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
        
        snprintf(formatted_message, sizeof(formatted_message), "[%s] [%s] %s",
                timestamp, logger_level_to_string(level), message);
    } else {
        snprintf(formatted_message, sizeof(formatted_message), "[%s] %s",
                logger_level_to_string(level), message);
    }
    
    // Log to console if enabled
    if (private_data->config.log_to_console) {
        printf("%s\n", formatted_message);
    }
    
    // Log to file if enabled
    if (private_data->config.log_to_file && private_data->log_file) {
        fprintf(private_data->log_file, "%s\n", formatted_message);
        fflush(private_data->log_file);
        
        // Update file size
        private_data->current_file_size += strlen(formatted_message) + 1; // +1 for newline
        
        // Check if we need to rotate the log file
        if (private_data->current_file_size >= private_data->config.max_file_size_kb * 1024) {
            logger_rotate_log_file();
        }
    }
    
    return true;
}

bool logger_log_sensor_data(const Sensor* sensor, const SensorData* data, LogLevel level) {
    if (!private_data || !sensor || !data || level < private_data->config.min_level) {
        return false;
    }
    
    // Create log entry
    LogEntry entry;
    entry.timestamp = (uint32_t)time(NULL);
    entry.level = level;
    strncpy(entry.sensor_id, sensor->id, sizeof(entry.sensor_id) - 1);
    entry.sensor_id[sizeof(entry.sensor_id) - 1] = '\0';
    strncpy(entry.sensor_type, sensor_type_to_string(sensor->type), sizeof(entry.sensor_type) - 1);
    entry.sensor_type[sizeof(entry.sensor_type) - 1] = '\0';
    entry.value = data->value;
    strncpy(entry.unit, data->unit, sizeof(entry.unit) - 1);
    entry.unit[sizeof(entry.unit) - 1] = '\0';
    entry.is_valid = data->is_valid;
    
    if (data->error != SENSOR_ERROR_NONE) {
        strncpy(entry.error, sensor_error_to_string(data->error), sizeof(entry.error) - 1);
    } else {
        strncpy(entry.error, "No Error", sizeof(entry.error) - 1);
    }
    entry.error[sizeof(entry.error) - 1] = '\0';
    
    // Format message
    snprintf(entry.message, sizeof(entry.message), 
             "Sensor: %s, Type: %s, Value: %.2f%s, Valid: %s, Error: %s",
             entry.sensor_id, entry.sensor_type, entry.value, entry.unit,
             entry.is_valid ? "Yes" : "No", entry.error);
    
    // Log the entry
    return logger_log(level, entry.message);
}

const char* logger_level_to_string(LogLevel level) {
    if (level >= sizeof(level_strings) / sizeof(level_strings[0])) {
        return "UNKNOWN";
    }
    return level_strings[level];
}

LogLevel logger_string_to_level(const char* level_str) {
    if (!level_str) {
        return LOG_LEVEL_INFO;
    }
    
    for (int i = 0; i < sizeof(level_strings) / sizeof(level_strings[0]); i++) {
        if (strcasecmp(level_str, level_strings[i]) == 0) {
            return (LogLevel)i;
        }
    }
    
    return LOG_LEVEL_INFO;
}

void logger_set_config(const LoggerConfig* config) {
    if (!private_data || !config) {
        return;
    }
    
    memcpy(&private_data->config, config, sizeof(LoggerConfig));
}

void logger_get_config(LoggerConfig* config) {
    if (!private_data || !config) {
        return;
    }
    
    memcpy(config, &private_data->config, sizeof(LoggerConfig));
}

void logger_rotate_log_file(void) {
    if (!private_data || !private_data->log_file) {
        return;
    }
    
    // Close current log file
    fclose(private_data->log_file);
    private_data->log_file = NULL;
    
    // Rotate existing log files
    for (int i = private_data->config.max_files - 1; i >= 0; i--) {
        char old_name[128];
        char new_name[128];
        
        if (i == 0) {
            strncpy(old_name, private_data->config.log_file, sizeof(old_name) - 1);
            old_name[sizeof(old_name) - 1] = '\0';
        } else {
            snprintf(old_name, sizeof(old_name), "%s.%d", 
                    private_data->config.log_file, i);
        }
        
        snprintf(new_name, sizeof(new_name), "%s.%d", 
                private_data->config.log_file, i + 1);
        
        // Remove the last file if it exists
        if (i == private_data->config.max_files - 1) {
            remove(old_name);
        } else {
            // Rename file
            rename(old_name, new_name);
        }
    }
    
    // Open new log file
    private_data->log_file = fopen(private_data->config.log_file, "w");
    if (private_data->log_file) {
        private_data->current_file_size = 0;
        logger_log(LOG_LEVEL_INFO, "Log file rotated");
    }
} 