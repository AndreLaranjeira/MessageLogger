// Message logger module - Header file.

// Define guard:
#ifndef MESSAGE_LOGGER_H_
#define MESSAGE_LOGGER_H_

// Includes:
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Macros:
#define DEFAULT_LOGGER_MESSAGE_COLORS { \
  [DEFAULT_MSG] = {                     \
    .text_color = DFLT,                 \
    .background_color = DFLT            \
  },                                    \
  [ERROR_MSG] = {                       \
    .text_color = DFLT,                 \
    .background_color = DFLT            \
  },                                    \
  [INFO_MSG] = {                        \
    .text_color = DFLT,                 \
    .background_color = DFLT            \
  },                                    \
  [SUCCESS_MSG] = {                     \
    .text_color = DFLT,                 \
    .background_color = DFLT            \
  },                                    \
  [WARNING_MSG] = {                     \
    .text_color = DFLT,                 \
    .background_color = DFLT            \
  }                                     \
}
#define DEFAULT_LOGGER_TAG_COLORS { \
  [CONTEXT_TAG] = {                 \
    .text_color = B_WHT,            \
    .background_color = DFLT        \
  },                                \
  [ERROR_TAG] = {                   \
    .text_color = B_RED,            \
    .background_color = DFLT        \
  },                                \
  [INFO_TAG] = {                    \
    .text_color = B_BLU,            \
    .background_color = DFLT        \
  },                                \
  [SUCCESS_TAG] = {                 \
    .text_color = B_GRN,            \
    .background_color = DFLT        \
  },                                \
  [WARNING_TAG] = {                 \
    .text_color = B_YEL,            \
    .background_color = DFLT        \
  }                                 \
}
#define TIME_FMT_SIZE 50

// Enumerations:
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
  B_WHT,
  DFLT
} Color;

typedef enum {
  WRITE,            // Write to log file.
  APPEND            // Append to log file.
} LogFileMode;

typedef enum {
  DEFAULT_MSG,
  ERROR_MSG,
  INFO_MSG,
  SUCCESS_MSG,
  WARNING_MSG
} MessageCategory;

typedef enum {
  CONTEXT_TAG,
  ERROR_TAG,
  INFO_TAG,
  SUCCESS_TAG,
  WARNING_TAG
} TagCategory;

// Enumeration companion macros:
#define NUM_OF_MESSAGE_CATEGORIES 5
#define NUM_OF_TAG_CATEGORIES 5

// Type definitions:
typedef struct {
  Color background_color;
  Color text_color;
} DisplayColors;

typedef struct {
  DisplayColors message_colors[NUM_OF_MESSAGE_CATEGORIES];
  DisplayColors tag_colors[NUM_OF_TAG_CATEGORIES];
} LoggerColorPallet;

typedef struct {
  char string_representation[TIME_FMT_SIZE];
} TimeFormat;

// Public function headers:
int configure_log_file(const char*, LogFileMode);
int enable_thread_safety();
int get_logger_msg_colors(DisplayColors*, MessageCategory);
int get_logger_tag_colors(DisplayColors*, TagCategory);
int get_time_format(TimeFormat*);
int set_logger_msg_colors(MessageCategory, DisplayColors*);
int set_logger_tag_colors(TagCategory, DisplayColors*);
int set_time_format(const char*);
void color_background(Color);
void color_text(Color);
void error(const char*, const char*, ...);
void info(const char*, const char*, ...);
void lock_logger_recursive_mutex();
void logger_module_clean_up();
void message(const char*, const char*, ...);
void reset_background_color();
void reset_colors();
void reset_text_color();
void success(const char*, const char*, ...);
void unlock_logger_recursive_mutex();
void warning(const char*, const char*, ...);

#endif // MESSAGE_LOGGER_H_
