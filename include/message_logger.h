// Message logger module - Header file.

// Define guard:
#ifndef MESSAGE_LOGGER_H_
#define MESSAGE_LOGGER_H_

// Includes:
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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
} color;

// Public function headers:
void color_background(color);
void color_text(color);
void error(const char*, const char*, ...);
void info(const char*, const char*, ...);
void message(const char*, const char*, ...);
void reset_colors();
void success(const char*, const char*, ...);
void warning(const char*, const char*, ...);

#endif // MESSAGE_LOGGER_H_
