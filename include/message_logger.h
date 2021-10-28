// Copyright (c) 2019 André Filipe Caldas Laranjeira
// MIT License

// Message Logger module - Header file.
//! \file message_logger.h
//! \author André Filipe Caldas Laranjeira
//! \brief Message Logger module - Header file.
//!
//! The Message Logger module header file contains all macros, enumerations,
//! types and function headers provided by the Message Logger module.

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

//! \def DEFAULT_LOGGER_MESSAGE_COLORS
//! \brief Macro to initialize a DisplayColors array with the default values
//! used by the Message Logger for messages.
//!
//! The array's length is expected to be equal to #NUM_OF_MESSAGE_CATEGORIES.
//!
//! Usage example:
//! \code
//! LoggerColorPallet color_pallet = {
//!   .message_colors = DEFAULT_LOGGER_MESSAGE_COLORS,
//!   // Initialize other LoggerColorPallet members...
//! };
//! \endcode
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

//! \def DEFAULT_LOGGER_TAG_COLORS
//! \brief Macro to initialize a DisplayColors array with the default values
//! used by the Message Logger for message tags.
//!
//! The array's length is expected to be equal to #NUM_OF_TAG_CATEGORIES.
//!
//! Usage example:
//! \code
//! LoggerColorPallet color_pallet = {
//!   .tag_colors = DEFAULT_LOGGER_TAG_COLORS,
//!   // Initialize other LoggerColorPallet members...
//! };
//! \endcode
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

//! \def TIME_FMT_SIZE
//! \brief Char length of the string_representation member of a TimeFormat.
//!
//! Use this macro when storing a time format string representation outside of
//! a TimeFormat variable or when copying a string representation to a
//! TimeFormat variable.
#define TIME_FMT_SIZE 50

// Enumerations:

//! \enum Color
//! \brief A color supported for display by a terminal.
//!
//! Represents a color that can be displayed by a terminal via an ANSI escape
//! code. Included are the 8 standard colors, their bright variants and the
//! default color used by a given terminal.
typedef enum {
  BLA,          //!< Black.
  RED,          //!< Red.
  GRN,          //!< Green.
  YEL,          //!< Yellow.
  BLU,          //!< Blue.
  MAG,          //!< Magenta.
  CYN,          //!< Cyan.
  WHT,          //!< White.
  B_BLA,        //!< Bright black.
  B_RED,        //!< Bright red.
  B_GRN,        //!< Bright green.
  B_YEL,        //!< Bright yellow.
  B_BLU,        //!< Bright blue.
  B_MAG,        //!< Bright magenta.
  B_CYN,        //!< Bright cyan.
  B_WHT,        //!< Bright white.
  DFLT          //!< Default color according to terminal settings.
} Color;

//! \enum LogFileMode
//! \brief A file mode used to open a log file.
//!
//! Similarly to the mode argument passed when opening any regular file, this
//! enumeration determines the behavior taken when a file with the same name as
//! the one you are trying to open already exists.
typedef enum {
  WRITE,        //!< Write to log file, overwriting any existing file.
  APPEND        //!< Append to log file, assuming one already exists.
} LogFileMode;

//! \enum MessageCategory
//! \brief A category of message supported by the Message Logger.
//!
//! Includes any and every different message type that is recognized by the
//! Message Logger module, including the default message. The number of
//! elements in this enumeration is stored in the #NUM_OF_MESSAGE_CATEGORIES
//! companion macro.
typedef enum {
  DEFAULT_MSG,        //!< Default message. No further significance.
  ERROR_MSG,          //!< Error message. Signifies an untreatable error.
  INFO_MSG,           //!< Info message. Provides complementary information.
  SUCCESS_MSG,        //!< Success message. Signifies a successful task.
  WARNING_MSG         //!< Warning message. Signifies a treatable error.
} MessageCategory;

//! \enum TagCategory
//! \brief A category of message tag supported by the Message Logger.
//!
//! Includes any and every different message tag type that is recognized by the
//! Message Logger module, including the context tag. The number of
//! elements in this enumeration is stored in the #NUM_OF_TAG_CATEGORIES
//! companion macro.
typedef enum {
  CONTEXT_TAG,  //!< Context tag. Adds call context. Used by all message types.
  ERROR_TAG,    //!< Error tag. Prefixes error messages.
  INFO_TAG,     //!< Info tag. Prefixes info messages.
  SUCCESS_TAG,  //!< Success tag. Prefixes success messages.
  WARNING_TAG   //!< Warning tag. Prefixes warning messages.
} TagCategory;

// Enumeration companion macros:

//! \def NUM_OF_MESSAGE_CATEGORIES
//! \brief Number of message categories supported by the Message Logger.
//!
//! Companion macro to the #MessageCategory enumeration. Use it as the size of
//! an array that should be accessed with a #MessageCategory as an index.
//!
//! Usage example:
//! \code
//! unsigned int message_log_quantity[NUM_OF_MESSAGE_CATEGORIES];
//! message_log_quantity[DEFAULT_MSG] = 0;
//! \endcode
#define NUM_OF_MESSAGE_CATEGORIES 5

//! \def NUM_OF_TAG_CATEGORIES
//! \brief Number of message tag categories supported by the Message Logger.
//!
//! Companion macro to the #TagCategory enumeration. Use it as the size of an
//! array that should be accessed with a #TagCategory as an index.
//!
//! Usage example:
//! \code
//! char tag_text[NUM_OF_TAG_CATEGORIES][100];
//! strcpy(tag_text[ERROR_TAG], "Critical error");
//! \endcode
#define NUM_OF_TAG_CATEGORIES 5

// Type definitions:

//! \struct DisplayColors
//! \brief Colors used when displaying text information on the terminal.
//!
//! When displaying messages or tags on the terminal, the Message Logger module
//! must known the colors to use both for the text font and for the background.
//! The colors used can vary in different message or tag types.
typedef struct {
  Color background_color;       //!< Color used for the text's background.
  Color text_color;             //!< Color used for the text's font.
} DisplayColors;

//! \struct LoggerColorPallet
//! \brief Display color information for all categories of messages and tags.
//!
//! This structure contains the display color information for each and every
//! type of message and tag utilized by the Message Logger module. Access to
//! information contained in the #message_colors and #tag_colors arrays should
//! be done using the #MessageCategory and #TagCategory enumerations,
//! respectively, as exemplified below:
//! \n
//! \code
//! LoggerColorPallet color_pallet = {
//!   // Assign color pallet members some values...
//! };
//! Color color1 = color_pallet.message_colors[DEFAULT_MSG].text_color;
//! Color color2 = color_pallet.tag_colors[ERROR_TAG].background_color;
//! \endcode
typedef struct {
  //! Display colors for all message categories in #MessageCategory.
  DisplayColors message_colors[NUM_OF_MESSAGE_CATEGORIES];
  //! Display colors for all tag categories in #TagCategory.
  DisplayColors tag_colors[NUM_OF_TAG_CATEGORIES];
} LoggerColorPallet;

//! \struct TimeFormat
//! \brief Time formatting information for storing messages in log files.
//!
//! When a log file is configured, the %TimeFormat information is necessary to
//! determine how the message's time will be written into the log file.
typedef struct {
  //! String specifying the time format for logs. Max length is #TIME_FMT_SIZE.
  char string_representation[TIME_FMT_SIZE];
} TimeFormat;

// Public function headers:
int configure_log_file(const char*, LogFileMode);
int enable_thread_safety();
int get_logger_msg_colors(DisplayColors*, MessageCategory);
int get_logger_tag_colors(DisplayColors*, TagCategory);
int get_time_format(TimeFormat*);
int set_logger_msg_colors(MessageCategory, const DisplayColors*);
int set_logger_tag_colors(TagCategory, const DisplayColors*);
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
void reset_logger_colors();
void reset_text_color();
void success(const char*, const char*, ...);
void unlock_logger_recursive_mutex();
void warning(const char*, const char*, ...);

#endif // MESSAGE_LOGGER_H_
