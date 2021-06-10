// Message logger module - Header file.

// Define guard:
#ifndef MESSAGE_LOGGER_H_
#define MESSAGE_LOGGER_H_

// Includes:
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Enums:
typedef enum {
  BLA,
  RED,
  GRN,
  YEL,
  BLU,
  MAG,
  CYN,
  WHT,
  B_BLA,
  B_RED,
  B_GRN,
  B_YEL,
  B_BLU,
  B_MAG,
  B_CYN,
  B_WHT
} Color;

typedef enum {
  WRITE,            // Write to log file.
  APPEND            // Append to log file.
} LogFileMode;

// Public function headers:
int configure_log_file(const char*, LogFileMode);
int enable_thread_safety();
void color_background(Color);
void color_text(Color);
void error(const char*, const char*, ...);
void info(const char*, const char*, ...);
void logger_module_clean_up();
void message(const char*, const char*, ...);
void reset_background_color();
void reset_colors();
void reset_text_color();
void success(const char*, const char*, ...);
void warning(const char*, const char*, ...);

#endif // MESSAGE_LOGGER_H_
