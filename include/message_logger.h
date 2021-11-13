// Copyright (c) 2019 André Filipe Caldas Laranjeira
// MIT License

// Message Logger module - Header file.
//! \file message_logger.h
//! \author André Filipe Caldas Laranjeira
//! \brief Message Logger module - Header file.
//!
//! The Message Logger module header file contains all macros, enumerations,
//! types and function prototypes provided by the Message Logger module.

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
//! \par Usage example
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
//! \par Usage example
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
//! \par Usage example
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
//! \par Usage example
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

// Public function prototypes:

//! \fn int configure_log_file(const char *file_name, LogFileMode file_mode)
//! \brief Configure a log file to store the Message Logger's messages.
//! Allocates resources, requiring a call to logger_module_clean_up()
//! afterwards.
//! \param file_name Name of the file used to log messages.
//! \param file_mode Mode used for opening the log file.
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function configures the Message Logger module to write any messages to
//! a log file, in adition to priting them on the screen. Each message written
//! on the log file has the message's contents and a timestamp.
//!
//! If an error occurs when configuring the log file, this function will return
//! -1 and the Message Logger will print an error message explaining what went
//! wrong.
//!
//! \note After the Message Logger module is no longer used, the funciton
//! logger_module_clean_up() must be called to close the log file created.
//!
//! \par Usage example
//! \code
//! configure_log_file("logger-test.log", WRITE);
//! // Use the Message Logger normally...
//! logger_module_clean_up();
//! \endcode
int configure_log_file(const char *file_name, LogFileMode file_mode);

//! \fn int enable_thread_safety()
//! \brief Enable thread safety for the Message Logger's operations. Allocates
//! resources, requiring a call to logger_module_clean_up() afterwards.
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function configures the Message Logger module to enable thread safety
//! when logging messages or executing other logger operations. This allows the
//! logger to be utilized in a multi-threaded environment with the pthreads
//! library without the nuissance of race conditions or other problems.
//!
//! If an error occurs when enabling thread safety, this function will return
//! -1 and the Message Logger will print an error message explaining what went
//! wrong.
//!
//! \note After the Message Logger module is no longer used, the function
//! logger_module_clean_up() must be called to release the memory allocated for
//! the thread-safety data structures utilized.
//!
//! \par Usage example
//! \code
//! // In the main thread, before creating other threads, enable thread safety.
//! enable_thread_safety();
//!
//! // ...
//! // Create multiple threads to use the Message Logger...
//!
//! // ...
//! // In the main thread, after other threads have been joined (and the
//! // logger is no longer used), call the clean up function.
//! logger_module_clean_up();
//! \endcode
int enable_thread_safety();

//! \fn int get_logger_msg_colors(
//!   DisplayColors* display_colors_destination,
//!   MessageCategory requested_category
//! )
//! \brief Get the display colors in the Message Logger for a message category.
//! \param display_colors_destination Pointer to where the requested display
//! colors are copied to. Must NOT be NULL.
//! \param requested_category Category of message whose display colors will be
//! copied.
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function returns the DisplayColors currently assigned to a
//! #MessageCategory in the Message Logger. The DisplayColors information is
//! copied from the logger's LoggerColorPallet to the pointer provided by the
//! user.
//!
//! If an error occurs when getting the display colors, this function will
//! return -1 and the Message Logger will print an error message explaining
//! what went wrong.
//!
//! \warning This function MUST receive a non-NULL DisplayColors pointer to
//! copy the information to. This function does NOT allocate memory on the
//! pointer provided by the user.
//!
//! \par Usage example
//! \code
//! DisplayColors current_success_message_colors;
//! get_logger_msg_colors(&current_success_message_colors, SUCCESS_MSG);
//! \endcode
int get_logger_msg_colors(
  DisplayColors* display_colors_destination,
  MessageCategory requested_category
);

//! \fn int get_logger_tag_colors(
//!   DisplayColors* display_colors_destination,
//!   TagCategory requested_category
//! )
//! \brief Get the display colors in the Message Logger for a tag category.
//! \param display_colors_destination Pointer to where the requested display
//! colors are copied to. Must NOT be NULL.
//! \param requested_category Category of tag whose display colors will be
//! copied.
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function returns the DisplayColors currently assigned to a
//! #TagCategory in the Message Logger. The DisplayColors information is copied
//! from the logger's LoggerColorPallet to the pointer provided by the user.
//!
//! If an error occurs when getting the display colors, this function will
//! return -1 and the Message Logger will print an error message explaining
//! what went wrong.
//!
//! \warning This function MUST receive a non-NULL DisplayColors pointer to
//! copy the information to. This function does NOT allocate memory on the
//! pointer provided by the user.
//!
//! \par Usage example
//! \code
//! DisplayColors current_success_tag_colors;
//! get_logger_tag_colors(&current_success_tag_colors, SUCCESS_TAG);
//! \endcode
int get_logger_tag_colors(
  DisplayColors* display_colors_destination,
  TagCategory requested_category
);

//! \fn int get_time_format(TimeFormat *time_format_destination)
//! \brief Get the time format used by the Message Logger in log files.
//! \param time_format_destination Pointer to where the requested time format
//! is copied to. Must NOT be NULL.
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function returns the TimeFormat currently used by the Message Logger
//! to determine how to format the time information written to log files. The
//! TimeFormat information is copied from the logger's internal variables to
//! the pointer provided by the user.
//!
//! If an error occurs when getting the time format, this function will return
//! -1 and the Message Logger will print an error message explaining what went
//! wrong.
//!
//! \warning This function MUST receive a non-NULL TimeFormat pointer to copy
//! the information to. This function does NOT allocate memory on the pointer
//! provided by the user.
//!
//! \par Usage example
//! \code
//! TimeFormat logger_time_format;
//! get_time_format(&logger_time_format);
//! \endcode
int get_time_format(TimeFormat *time_format_destination);

//! \fn int set_logger_msg_colors(
//!   MessageCategory message_category,
//!   const DisplayColors *assigned_colors
//! )
//! \brief Set the display colors used by the Message Logger for a message
//! category.
//! \param message_category Category of message whose display colors are being
//! set.
//! \param assigned_colors Pointer to the display colors being copied to a
//! message category of the \link #logger_color_pallet Message Logger's
//! color pallet. \endlink
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function sets the display colors for a given #MessageCategory in the
//! \link #logger_color_pallet Message Logger's color pallet. \endlink This is
//! done by copying the information from a valid, non-NULL DisplayColors
//! pointer provided by the user to an index of the \link
//! LoggerColorPallet::message_colors message_colors \endlink array.
//!
//! If an error occurs when setting the display colors, this function will
//! return -1 and the Message Logger will print an error message explaining
//! what went wrong.
//!
//! \par Usage example
//! \code
//! DisplayColors custom_info_msg_colors = {
//!   .text_color = B_WHT,
//!   .background_color = CYN
//! };
//! set_logger_msg_colors(INFO_MSG, &custom_info_msg_colors);
//! \endcode
int set_logger_msg_colors(
  MessageCategory message_category,
  const DisplayColors *assigned_colors
);

//! \fn int set_logger_tag_colors(
//!   TagCategory tag_category,
//!   const DisplayColors *assigned_colors
//! )
//! \brief Set the display colors used by the Message Logger for a tag
//! category.
//! \param tag_category Category of tag whose display colors are being set.
//! \param assigned_colors Pointer to the display colors being copied to a tag
//! category of the \link #logger_color_pallet Message Logger's
//! color pallet. \endlink
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function sets the display colors for a given #TagCategory in the \link
//! #logger_color_pallet Message Logger's color pallet. \endlink This is done
//! by copying the information from a valid, non-NULL DisplayColors pointer
//! provided by the user to an index of the \link LoggerColorPallet::tag_colors
//! tag_colors \endlink array.
//!
//! If an error occurs when setting the display colors, this function will
//! return -1 and the Message Logger will print an error message explaining
//! what went wrong.
//!
//! \par Usage example
//! \code
//! DisplayColors custom_info_tag_colors = {
//!   .text_color = B_BLA,
//!   .background_color = CYN
//! };
//! set_logger_tag_colors(INFO_TAG, &custom_info_tag_colors);
//! \endcode
int set_logger_tag_colors(
  TagCategory tag_category,
  const DisplayColors *assigned_colors
);

//! \fn int set_time_format(const char *new_format)
//! \brief Set the time format used by the Message Logger in log files.
//! \param new_format String representation of the time format to be used. Size
//! must be smaller or equal to #TIME_FMT_SIZE.
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function sets the \link TimeFormat::string_representation
//! string_representation \endlink in the \link #logger_time_fmt
//! Message Logger's time format. \endlink This format is used when
//! timestamping messages saved to a log file, if one is configured. The time
//! format information is copied from a valid, non-NULL pointer provided by the
//! user.
//!
//! If an error occurs when setting the time format, this function will return
//! -1 and the Message Logger will print an error message explaining what went
//! wrong.
//!
//! \par Usage example
//! \code
//! set_time_format("%c");
//! \endcode
int set_time_format(const char *new_format);

//! \fn void color_background(Color p_color)
//! \brief Changes the terminal text's background color to a specific #Color.
//! \param p_color Color to be applied to the terminal text's background.
//!
//! This function changes the terminal text's background color to a #Color
//! provided by the user. When line breaks occur, the entire line's background
//! is colored, not just the portion with text. If this function is called in
//! the middle of a text line, any existing background color will be cleared
//! after the cursor's position.
//!
//! \par Usage example
//! \code
//! color_background(B_GRN);
//! \endcode
void color_background(Color p_color);

//! \fn void color_text(Color p_color)
//! \brief Changes the terminal text's font color to a specific #Color.
//! \param p_color Color to be applied to the terminal text's font.
//!
//! This function changes the terminal text's font color to a #Color provided by
//! the user. Bright colors are accompanied by a bold font weight, while the
//! other font colors are accompanied by a regular font weight.
//!
//! \par Usage example
//! \code
//! color_text(BLU);
//! \endcode
void color_text(Color p_color);

//! \fn void error(const char *context, const char *format, ...)
//! \brief Log an error message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function writes an error message to the terminal and configured log
//! file. The message contains the caller context that originated the error, a
//! tag that identifies the message as an error and the message's contents. If
//! a log file is configured, the message will also have the timestamp
//! information when written to the log file.
//!
//! \par Usage example
//! \code
//! int arg = 0;
//! error("Example", "%d: This is an error message with a context.\n", arg);
//! error(NULL, "%d: This is also an error message.\n", arg+1);
//! \endcode
void error(const char *context, const char *format, ...);

//! \fn void info(const char *context, const char *format, ...)
//! \brief Log an info message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function writes an info message to the terminal and configured log file.
//! The message contains the caller context that originated the info message, a
//! tag that identifies the message as an info message and the message's
//! contents. If a log file is configured, the message will also have the
//! timestamp information when written to the log file.
//!
//! \par Usage example
//! \code
//! int arg = 0;
//! info("Example", "%d: This is an info message with a context.\n", arg);
//! info(NULL, "%d: This is also an info message.\n", arg+1);
//! \endcode
void info(const char *context, const char *format, ...);

//! \fn void lock_logger_recursive_mutex()
//! \brief Lock the \link #logger_recursive_mutex Message Logger's recursive
//! mutex lock. \endlink This prevents any other thread from using the Message
//! Logger. Thread safety MUST be enabled.
//!
//! This function locks the recursive mutex lock utilized by the Message Logger
//! to ensure thread safety. When the recursive mutex is locked by a thread,
//! the other threads will not be able to lock it as well and will be suspended
//! until the mutex is unlocked. This ensures that no race conditions take
//! place. The recursive nature of the mutex lock allows a thread to lock it
//! more than once without being suspended.
//!
//! Since most of the Message Logger's functions already handle the recursive
//! mutex lock when thread safety is enabled, the main purpose of this function
//! is to allow the user's code to avoid race conditions with the logger. For
//! example, if the user wishes to print information on the terminal without
//! using the Message Logger, this function will guarantee that the logger
//! doesn't try to do the same from another thread while the user's code is
//! running.
//!
//! \warning For this function to work properly, the function
//! enable_thread_safety() must have been called to configure the recursive
//! mutex lock. Otherwise, this function will emit a warning and will have NO
//! EFFECT.
//!
//! \par Usage example
//! \code
//! // In the main thread, before creating other threads, enable thread safety.
//! enable_thread_safety();
//!
//! // ...
//! // After creating multiple threads, in one of the threads.
//! lock_logger_recursive_mutex();
//! printf("This message will NOT interfere with the logger's operations!\n");
//! unlock_logger_recursive_mutex();
//!
//! // ...
//! // In the main thread, after other threads have been joined (and the
//! // logger is no longer used), call the clean up function.
//! logger_module_clean_up();
//! \endcode
void lock_logger_recursive_mutex();

//! \fn void logger_module_clean_up()
//! \brief Clean up the resources allocated by the Message Logger.
//!
//! This function cleans up any memory or other resources utilized by the
//! Message Logger. Ideally, it should always be called after the logger is not
//! longer utilized. If this function is called and the logger is utilized
//! afterwards, some configurations such as the log file and thread safety will
//! NOT work.
//!
//! \warning This function NEEDS to be called when a log file is configured or
//! when thread safety is enabled. Failure to do so might result in an
//! incomplete log file or memory leaks.
//!
//! \par Usage example
//! \code
//! // Use the logger normally...
//! logger_module_clean_up();
//! \endcode
void logger_module_clean_up();

//! \fn void message(const char *context, const char *format, ...)
//! \brief Log a regular message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function writes a regular message to the terminal and configured log
//! file. The message contains the caller context that originated the regular
//! message and the message's contents. If a log file is configured, the
//! message will also have the timestamp information when written to the log
//! file.
//!
//! \par Usage example
//! \code
//! int arg = 0;
//! message("Example", "%d: This is a normal message with a context.\n", arg);
//! message(NULL, "%d: This is also a normal message.\n", arg+1);
//! \endcode
void message(const char *context, const char *format, ...);

//! \fn void reset_background_color()
//! \brief Reset the terminal's text background color to the default color.
//!
//! \deprecated This function will be removed in the next major release. Use
//! color_background(DFLT) instead.
//!
//! This function resets the terminal's text background color to the default
//! color specified in the terminal's configuration.
//!
//! \par Usage example
//! \code
//! // Change the text background color in some way.
//! reset_background_color();
//! \endcode
void reset_background_color();

//! \fn void reset_colors()
//! \brief Reset all the terminal's colors and text attributes to their
//! defaults and clears any existing text background colors past the cursor.
//!
//! This function resets all of the terminal's colors and text attributes to
//! the default configurations specified for the terminal and clears any
//! existing text background colors past the cursor in the current line. The
//! attributes reset include the colors used for text font and text background,
//! the font weight and any other characteristics configured using ANSI escape
//! codes.
//!
//! \par Usage example
//! \code
//! // Change various text configurations.
//! reset_colors();
//! \endcode
void reset_colors();

//! \fn void reset_logger_colors()
//! \brief Reset the \link #logger_color_pallet Message Logger's color pallet
//! \endlink colors to their defaults.
//!
//! This function resets the \link #logger_color_pallet Message Logger's color
//! pallet \endlink by setting it's \link LoggerColorPallet::message_colors
//! message_colors \endlink and \link LoggerColorPallet::tag_colors tag_colors
//! \endlink to the default values specifed in the macros
//! #DEFAULT_LOGGER_MESSAGE_COLORS and #DEFAULT_LOGGER_TAG_COLORS. ALL the
//! values are reset, so any changes made with calls to the functions
//! \link set_logger_msg_colors() set_logger_msg_colors \endlink and \link
//! set_logger_tag_colors() set_logger_tag_colors \endlink will be lost after
//! this function is called.
//!
//! \par Usage example
//! \code
//! // Change various logger pallet colors.
//! reset_logger_colors();
//! \endcode
void reset_logger_colors();

//! \fn void reset_text_color()
//! \brief Reset the terminal's text font color to the default color.
//!
//! \deprecated This function will be removed in the next major release. Use
//! color_text(DFLT) instead.
//!
//! This function resets the terminal's text font color to the default color
//! specified in the terminal's configuration.
//!
//! \par Usage example
//! \code
//! // Change the text font color in some way.
//! reset_text_color();
//! \endcode
void reset_text_color();

//! \fn void success(const char *context, const char *format, ...)
//! \brief Log a success message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function writes a success message to the terminal and configured log
//! file. The message contains the caller context that originated the success
//! message, a tag that identifies the message as a success message and the
//! message's contents. If a log file is configured, the message will also have
//! the timestamp information when written to the log file.
//!
//! \par Usage example
//! \code
//! int arg = 0;
//! success("Example", "%d: This is a success message with a context.\n", arg);
//! success(NULL, "%d: This is also a success message.\n", arg+1);
//! \endcode
void success(const char *context, const char *format, ...);

//! \fn void unlock_logger_recursive_mutex()
//! \brief Unlock the \link #logger_recursive_mutex Message Logger's recursive
//! mutex lock \endlink, allowing other
//! threads to use the Message Logger if no recursive locks remain. Thread
//! safety MUST be enabled.
//!
//! This function unlocks the recursive mutex lock utilized by the Message
//! Logger to ensure thread safety. When the recursive mutex is locked by a
//! thread, the other threads will not be able to lock it as well and will be
//! suspended until the mutex is unlocked. This ensures that no race conditions
//! take place. The recursive nature of the mutex lock allows a thread to lock
//! it more than once without being suspended.
//!
//! Since most of the Message Logger's functions already handle the recursive
//! mutex lock when thread safety is enabled, the main purpose of this function
//! is to allow the user's code to avoid race conditions with the logger. For
//! example, if the user wishes to print information on the terminal without
//! using the Message Logger, this function will allow the logger to resume
//! it's operations in a thread safe manner after the user prints all
//! information to the screen.
//!
//! \warning For this function to work properly, the function
//! enable_thread_safety() must have been called to configure the recursive
//! mutex lock. Otherwise, this function will emit a warning and will have NO
//! EFFECT.
//!
//! \par Usage example
//! \code
//! // In the main thread, before creating other threads, enable thread safety.
//! enable_thread_safety();
//!
//! // ...
//! // After creating multiple threads, in one of the threads.
//! lock_logger_recursive_mutex();
//! printf("This message will NOT interfere with the logger's operations!\n");
//! unlock_logger_recursive_mutex();
//!
//! // ...
//! // In the main thread, after other threads have been joined (and the
//! // logger is no longer used), call the clean up function.
//! logger_module_clean_up();
//! \endcode
void unlock_logger_recursive_mutex();

//! \fn void warning(const char *context, const char *format, ...)
//! \brief Log a warning message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function writes a warning message to the terminal and configured log
//! file. The message contains the caller context that originated the warning,
//! a tag that identifies the message as a warning and the message's contents.
//! If a log file is configured, the message will also have the timestamp
//! information when written to the log file.
//!
//! \par Usage example
//! \code
//! int arg = 0;
//! warning("Example", "%d: This is a warning message with a context.\n", arg);
//! warning(NULL, "%d: This is also a warning message.\n", arg+1);
//! \endcode
void warning(const char *context, const char *format, ...);

#endif // MESSAGE_LOGGER_H_
