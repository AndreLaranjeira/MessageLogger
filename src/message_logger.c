// Copyright (c) 2019 André Filipe Caldas Laranjeira
// MIT License

// Message logger module - Source code.
//! \file message_logger.c
//! \author André Filipe Caldas Laranjeira
//! \brief Message logger module - Source code.
//!
//! The Message Logger module source file contains the implementation of all
//! the functions provided by the Message Logger module and private state
//! variables.

// Includes:
#include "message_logger.h"

// Private constants:
const static LoggerColorPallet default_color_pallet = {
  .message_colors = DEFAULT_LOGGER_MESSAGE_COLORS,
  .tag_colors = DEFAULT_LOGGER_TAG_COLORS
};

// Private variables:
static char time_fmt[TIME_FMT_SIZE] = "%H:%M:%S %d-%m-%Y";
static FILE *log_file = NULL;
static LoggerColorPallet logger_color_pallet = {
  .message_colors = DEFAULT_LOGGER_MESSAGE_COLORS,
  .tag_colors = DEFAULT_LOGGER_TAG_COLORS
};
static pthread_mutex_t *logger_recursive_mutex = NULL;

// Private function headers:
static void apply_all_default_attributes();
static void clear_background_in_current_line();
static void copy_display_colors(DisplayColors*, const DisplayColors*);
static void log_datetime(FILE*, const char*);
static void log_formatted_text_content(FILE*, const char*, va_list);
static void log_message(FILE*, const char*, const char*, const char*,
                        const char*, va_list);
static void print_context(const char*);
static void print_formatted_text(const char*, va_list);

// Public function implementations:

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
//! on the log file has the message's contents and information on the message's
//! datetime.
//!
//! If an error occurs when configuring the log file, this function will return
//! -1 and the Message Logger will print an error message explaining what went
//! wrong.
//!
//! \b Important: After the Message Logger module is no longer used, the
//! function logger_module_clean_up() must be called to close the log file
//! created.
//!
//! Usage example:
//! \code
//! configure_log_file("logger-test.log", WRITE);
//! // Use the Message Logger normally...
//! logger_module_clean_up();
//! \endcode
int configure_log_file(const char *file_name , LogFileMode file_mode) {

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  // If there was a previous log file, we need to close it:
  if(log_file != NULL) {
    fclose(log_file);
    log_file = NULL;
  }

  // Open the log file and store it's pointer for future use:
  switch (file_mode) {

    case APPEND:
      log_file = fopen(file_name, "a");

      if(log_file == NULL) {
        warning(
          "Logger module",
          "Could not find log file! Defaulting to write mode!\n"
        );
        log_file = fopen(file_name, "w");
      }

      break;

    case WRITE:
      log_file = fopen(file_name, "w");
      break;

  }

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  if(log_file == NULL) {
    error(
      "Logger module",
      "Could not create log file! Please check your system.\n"
    );
    return -1;
  }

  return 0;

}

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
//! \b Important: After the Message Logger module is no longer used, the
//! function logger_module_clean_up() must be called to release the memory
//! allocated for the thread-safety data structures utilized.
//!
//! Usage example:
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
int enable_thread_safety() {

  pthread_mutexattr_t logger_mutex_attributes;

  // Allocate recursive mutex:
  logger_recursive_mutex = malloc(sizeof(pthread_mutex_t));

  if(logger_recursive_mutex == NULL) {
    error(
      "Logger module",
      "Could not allocate memory for mutex lock! Please check your system."
    );
    return -1;
  }

  // Initialize recursive mutex:
  pthread_mutexattr_init(&logger_mutex_attributes);
  pthread_mutexattr_settype(&logger_mutex_attributes, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(logger_recursive_mutex, &logger_mutex_attributes);
  pthread_mutexattr_destroy(&logger_mutex_attributes);

  return 0;

}

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
//! \b Important: This function MUST receive a non-NULL DisplayColors pointer
//! to copy the information to. This function does NOT allocate memory on the
//! pointer provided by the user.
//!
//! Usage example:
//! \code
//! DisplayColors current_success_message_colors;
//! get_logger_msg_colors(&current_success_message_colors, SUCCESS_MSG);
//! \endcode
int get_logger_msg_colors(
  DisplayColors* display_colors_destination,
  MessageCategory requested_category
) {

  if(display_colors_destination == NULL) {
    error(
      "Logger module",
      "Cannot store display color information in NULL pointer! "
      "Please use a valid reference.\n"
    );
    return -1;
  }

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  copy_display_colors(
    display_colors_destination,
    &logger_color_pallet.message_colors[requested_category]
  );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  return 0;

}

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
//! \b Important: This function MUST receive a non-NULL DisplayColors pointer
//! to copy the information to. This function does NOT allocate memory on the
//! pointer provided by the user.
//!
//! Usage example:
//! \code
//! DisplayColors current_success_tag_colors;
//! get_logger_tag_colors(&current_success_tag_colors, SUCCESS_TAG);
//! \endcode
int get_logger_tag_colors(
  DisplayColors* display_colors_destination,
  TagCategory requested_category
) {

  if(display_colors_destination == NULL) {
    error(
      "Logger module",
      "Cannot store display color information in NULL pointer! "
      "Please use a valid reference.\n"
    );
    return -1;
  }

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  copy_display_colors(
    display_colors_destination,
    &logger_color_pallet.tag_colors[requested_category]
  );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  return 0;

}

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
//! \b Important: This function MUST receive a non-NULL TimeFormat pointer
//! to copy the information to. This function does NOT allocate memory on the
//! pointer provided by the user.
//!
//! Usage example:
//! \code
//! TimeFormat logger_time_format;
//! get_time_format(&logger_time_format);
//! \endcode
int get_time_format(TimeFormat *time_format_destination) {

  if(time_format_destination == NULL) {
    error(
      "Logger module",
      "Cannot store time format in NULL pointer! "
      "Please use a valid reference.\n"
    );
    return -1;
  }

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  // Copy logger time format to destination time format:
  strncpy(
    time_format_destination->string_representation,
    time_fmt,
    TIME_FMT_SIZE
  );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  return 0;

}

//! \fn int set_logger_msg_colors(
//!   MessageCategory message_category,
//!   const DisplayColors *assigned_colors
//! )
//! \brief Set the display colors used by the Message Logger for a message
//! category.
//! \param message_category Category of message whose display colors are being
//! set.
//! \param assigned_colors Pointer to the display colors being copied to a
//! message category of the Message Logger's LoggerColorPallet.
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function sets the display colors for a given #MessageCategory in the
//! Message Logger's LoggerColorPallet. This is done by copying the information
//! from a valid, non-NULL DisplayColors pointer provided by the user to an
//! index of the \link LoggerColorPallet::message_colors message_colors
//! \endlink array.
//!
//! If an error occurs when setting the display colors, this function will
//! return -1 and the Message Logger will print an error message explaining
//! what went wrong.
//!
//! Usage example:
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
) {

  if(assigned_colors == NULL) {
    error(
      "Logger module",
      "Cannot assign display color information from NULL pointer! "
      "Please use a valid reference.\n"
    );
    return -1;
  }

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  copy_display_colors(
    &logger_color_pallet.message_colors[message_category],
    assigned_colors
  );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  return 0;

}

//! \fn int set_logger_tag_colors(
//!   TagCategory tag_category,
//!   const DisplayColors *assigned_colors
//! )
//! \brief Set the display colors used by the Message Logger for a tag
//! category.
//! \param tag_category Category of tag whose display colors are being set.
//! \param assigned_colors Pointer to the display colors being copied to a tag
//! category of the Message Logger's LoggerColorPallet.
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function sets the display colors for a given #TagCategory in the
//! Message Logger's LoggerColorPallet. This is done by copying the information
//! from a valid, non-NULL DisplayColors pointer provided by the user to an
//! index of the \link LoggerColorPallet::tag_colors tag_colors \endlink array.
//!
//! If an error occurs when setting the display colors, this function will
//! return -1 and the Message Logger will print an error message explaining
//! what went wrong.
//!
//! Usage example:
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
) {

  if(assigned_colors == NULL) {
    error(
      "Logger module",
      "Cannot assign display color information from NULL pointer! "
      "Please use a valid reference.\n"
    );
    return -1;
  }

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  copy_display_colors(
    &logger_color_pallet.tag_colors[tag_category],
    assigned_colors
  );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  return 0;

}

//! \fn int set_time_format(const char *new_format)
//! \brief Set the time format used by the Message Logger in log files.
//! \param new_format String representation of the time format to be used. Size
//! must be smaller or equal to #TIME_FMT_SIZE.
//! \return Returns 0 when successfully executed and -1 if an error occurs.
//!
//! This function sets the \link TimeFormat::string_representation
//! string_representation \endlink in the Message Logger's TimeFormat, which is
//! used when saving messages to a log file if one is configured. The time
//! format information is copied from a valid, non-NULL pointer provided by the
//! user.
//!
//! If an error occurs when setting the time format, this function will return
//! -1 and the Message Logger will print an error message explaining what went
//! wrong.
//!
//! Usage example:
//! \code
//! set_time_format("%c");
//! \endcode
int set_time_format(const char *new_format) {

  if(new_format == NULL) {
    error(
      "Logger module",
      "Cannot assign time format from a NULL pointer! "
      "Please use a valid reference.\n"
    );
    return -1;
  }

  if(strlen(new_format) > TIME_FMT_SIZE) {
    error(
      "Logger module",
      "Could not change time format! Try again with an argument of less "
      "then %u characters.\n",
      TIME_FMT_SIZE
    );
    return -1;
  }

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  // Copy new time format to logger time format:
  strncpy(time_fmt, new_format, TIME_FMT_SIZE);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  return 0;

}

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
//! Usage example:
//! \code
//! color_background(B_GRN);
//! \endcode
void color_background(Color p_color) {

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  switch(p_color) {

    case BLA:
      printf("\x1B[48;5;0m");
      break;

    case RED:
      printf("\x1B[48;5;1m");
      break;

    case GRN:
      printf("\x1B[48;5;2m");
      break;

    case YEL:
      printf("\x1B[48;5;3m");
      break;

    case BLU:
      printf("\x1B[48;5;4m");
      break;

    case MAG:
      printf("\x1B[48;5;5m");
      break;

    case CYN:
      printf("\x1B[48;5;6m");
      break;

    case WHT:
      printf("\x1B[48;5;7m");
      break;

    case B_BLA:
      printf("\x1B[48;5;8m");
      break;

    case B_RED:
      printf("\x1B[48;5;9m");
      break;

    case B_GRN:
      printf("\x1B[48;5;10m");
      break;

    case B_YEL:
      printf("\x1B[48;5;11m");
      break;

    case B_BLU:
      printf("\x1B[48;5;12m");
      break;

    case B_MAG:
      printf("\x1B[48;5;13m");
      break;

    case B_CYN:
      printf("\x1B[48;5;14m");
      break;

    case B_WHT:
      printf("\x1B[48;5;15m");
      break;

    case DFLT:
      printf("\x1B[49m");
      break;

  }

  clear_background_in_current_line();

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

}

//! \fn void color_text(Color p_color)
//! \brief Changes the terminal text's font color to a specific #Color.
//! \param p_color Color to be applied to the terminal text's font.
//!
//! This function changes the terminal text's font color to a #Color provided by
//! the user. Bright colors are accompanied by a bold font weight, while the
//! other font colors are accompanied by a regular font weight.
//!
//! Usage example:
//! \code
//! color_text(BLU);
//! \endcode
void color_text(Color p_color) {

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  switch(p_color) {

    case BLA:
      printf("\x1B[22;38;5;0m");
      break;

    case RED:
      printf("\x1B[22;38;5;1m");
      break;

    case GRN:
      printf("\x1B[22;38;5;2m");
      break;

    case YEL:
      printf("\x1B[22;38;5;3m");
      break;

    case BLU:
      printf("\x1B[22;38;5;4m");
      break;

    case MAG:
      printf("\x1B[22;38;5;5m");
      break;

    case CYN:
      printf("\x1B[22;38;5;6m");
      break;

    case WHT:
      printf("\x1B[22;38;5;7m");
      break;

    case B_BLA:
      printf("\x1B[1;38;5;8m");
      break;

    case B_RED:
      printf("\x1B[1;38;5;9m");
      break;

    case B_GRN:
      printf("\x1B[1;38;5;10m");
      break;

    case B_YEL:
      printf("\x1B[1;38;5;11m");
      break;

    case B_BLU:
      printf("\x1B[1;38;5;12m");
      break;

    case B_MAG:
      printf("\x1B[1;38;5;13m");
      break;

    case B_CYN:
      printf("\x1B[1;38;5;14m");
      break;

    case B_WHT:
      printf("\x1B[1;38;5;15m");
      break;

    case DFLT:
      printf("\x1B[22;39m");
      break;

  }

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

}

//! \fn void error(const char *context, const char *format, ...)
//! \brief Log an error message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function logs an error message to the terminal and configured log
//! file. The message contains the caller context that originated the error, a
//! tag that identifies the message as an error and the message's contents. If
//! a log file is configured, the message will also have the timestamp
//! information when written to the log file.
//!
//! Usage example:
//! \code
//! int arg = 0;
//! error("Example", "%d: This is an error message with a context.\n", arg);
//! error(NULL, "%d: This is also an error message.\n", arg+1);
//! \endcode
void error(const char *context, const char *format, ...) {

  const char msg_type[8] = "(Error)";
  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  color_text(logger_color_pallet.tag_colors[ERROR_TAG].text_color);
  color_background(logger_color_pallet.tag_colors[ERROR_TAG].background_color);
  printf("%s ", msg_type);

  // Print message contents:
  color_text(logger_color_pallet.message_colors[ERROR_MSG].text_color);
  color_background(
    logger_color_pallet.message_colors[ERROR_MSG].background_color
  );
  print_formatted_text(format, arg_list);

  // Reset display colors:
  reset_colors();

  // If a log file exists, write the message contents to it:
  if(log_file != NULL)
    log_message(log_file, time_fmt, context, msg_type, format, arg_list);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

//! \fn void info(const char *context, const char *format, ...)
//! \brief Log an info message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function logs an info message to the terminal and configured log file.
//! The message contains the caller context that originated the info message, a
//! tag that identifies the message as an info message and the message's
//! contents. If a log file is configured, the message will also have the
//! timestamp information when written to the log file.
//!
//! Usage example:
//! \code
//! int arg = 0;
//! info("Example", "%d: This is an info message with a context.\n", arg);
//! info(NULL, "%d: This is also an info message.\n", arg+1);
//! \endcode
void info(const char *context, const char *format, ...) {

  const char msg_type[7] = "(Info)";
  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  color_text(logger_color_pallet.tag_colors[INFO_TAG].text_color);
  color_background(logger_color_pallet.tag_colors[INFO_TAG].background_color);
  printf("%s ", msg_type);

  // Print message contents:
  color_text(logger_color_pallet.message_colors[INFO_MSG].text_color);
  color_background(
    logger_color_pallet.message_colors[INFO_MSG].background_color
  );
  print_formatted_text(format, arg_list);

  // Reset display colors:
  reset_colors();

  // If a log file exists, write the message contents to it:
  if(log_file != NULL)
    log_message(log_file, time_fmt, context, msg_type, format, arg_list);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

//! \fn void lock_logger_recursive_mutex()
//! \brief Lock the Message Logger's recursive mutex lock, preventing any
//! other thread from using the Message Logger. Thread safety MUST be enabled.
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
//! \b Important: For this function to work properly, the function
//! enable_thread_safety() must have been called to configure the recursive
//! mutex lock. Otherwise, this function will emit a warning and will have NO
//! EFFECT.
//!
//! Usage example:
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
void lock_logger_recursive_mutex() {
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  else
    warning(
      "Logger module",
      "Enable thread safety to access the logger recursive mutex."
    );
}

//! \fn void logger_module_clean_up()
//! \brief Clean up the resources allocated by the Message Logger.
//!
//! This function cleans up any memory or other resources utilized by the
//! Message Logger. Ideally, it should always be called after the logger is not
//! longer utilized. If this function is called and the logger is utilized
//! afterwards, some configurations such as the log file and thread safety will
//! NOT work.
//!
//! \b Important: This function NEEDS to be called when a log file is
//! configured or when thread safety is enabled. Failure to do so might result
//! in an incomplete log file or memory leaks.
//!
//! Usage example:
//! \code
//! // Use the logger normally...
//! logger_module_clean_up();
//! \endcode
void logger_module_clean_up() {

  // Clean up the log file:
  if(log_file != NULL) {
    fclose(log_file);
    log_file = NULL;
  }

  // Clean up the recursive mutex:
  if(logger_recursive_mutex != NULL) {
    pthread_mutex_destroy(logger_recursive_mutex);
    free(logger_recursive_mutex);
    logger_recursive_mutex = NULL;
  }

}

//! \fn void message(const char *context, const char *format, ...)
//! \brief Log a regular message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function logs a regular message to the terminal and configured log
//! file. The message contains the caller context that originated the regular
//! message and the message's contents. If a log file is configured, the
//! message will also have the timestamp information when written to the log
//! file.
//!
//! Usage example:
//! \code
//! int arg = 0;
//! message("Example", "%d: This is a normal message with a context.\n", arg);
//! message(NULL, "%d: This is also a normal message.\n", arg+1);
//! \endcode
void message(const char *context, const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print message contents:
  color_text(logger_color_pallet.message_colors[DEFAULT_MSG].text_color);
  color_background(
    logger_color_pallet.message_colors[DEFAULT_MSG].background_color
  );
  print_formatted_text(format, arg_list);

  // Reset display colors:
  reset_colors();

  // If a log file exists, write the message contents to it:
  if(log_file != NULL)
    log_message(log_file, time_fmt, context, NULL, format, arg_list);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

//! \fn void reset_background_color()
//! \brief Reset the terminal's text background color to the default color.
//!
//! <b>Deprecation warning:</b> This function will be removed in the next major
//! release. Use color_background(DFLT) instead.
//!
//! This function resets the terminal's text background color to the default
//! color specified in the terminal's configuration.
//!
//! Usage example:
//! \code
//! // Change the text background color in some way.
//! reset_background_color();
//! \endcode
void reset_background_color() {
  color_background(DFLT);
}

//! \fn void reset_colors()
//! \brief Reset all the terminal's colors and text attributes to their
//! defaults.
//!
//! This function resets all of the terminal's colors and text attributes to
//! the default configurations specified for the terminal. This includes the
//! colors used for text font and text background, the font weight and any
//! other characteristics configured using ANSI escape codes.
//!
//! Usage example:
//! \code
//! // Change various text configurations.
//! reset_colors();
//! \endcode
void reset_colors() {
  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  apply_all_default_attributes();
  clear_background_in_current_line();

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);
}

//! \fn void reset_logger_colors()
//! \brief Reset the Message Logger's colors to their defaults.
//!
//! This function resets the Message Logger's LoggerColorPallet by setting it's
//! \link LoggerColorPallet::message_colors message_colors \endlink and \link
//! LoggerColorPallet::tag_colors tag_colors \endlink to the default values
//! specifed in the macros #DEFAULT_LOGGER_MESSAGE_COLORS and
//! #DEFAULT_LOGGER_TAG_COLORS. ALL the values are reset, so any changes made
//! with calls to the functions \link set_logger_msg_colors()
//! set_logger_msg_colors \endlink and \link set_logger_tag_colors()
//! set_logger_tag_colors \endlink will be lost after this function is called.
//!
//! Usage example:
//! \code
//! // Change various logger pallet colors.
//! reset_logger_colors();
//! \endcode
void reset_logger_colors() {

  int i;

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  for(i = 0; i < NUM_OF_MESSAGE_CATEGORIES; i++) {
    copy_display_colors(
      &logger_color_pallet.message_colors[i],
      &default_color_pallet.message_colors[i]
    );
  }

  for(i = 0; i < NUM_OF_TAG_CATEGORIES; i++) {
    copy_display_colors(
      &logger_color_pallet.tag_colors[i],
      &default_color_pallet.tag_colors[i]
    );
  }

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);
}

//! \fn void reset_text_color()
//! \brief Reset the terminal's text font color to the default color.
//!
//! <b>Deprecation warning:</b> This function will be removed in the next major
//! release. Use color_text(DFLT) instead.
//!
//! This function resets the terminal's text font color to the default color
//! specified in the terminal's configuration.
//!
//! Usage example:
//! \code
//! // Change the text font color in some way.
//! reset_text_color();
//! \endcode
void reset_text_color() {
  color_text(DFLT);
}

//! \fn void success(const char *context, const char *format, ...)
//! \brief Log a success message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function logs a success message to the terminal and configured log
//! file. The message contains the caller context that originated the success
//! message, a tag that identifies the message as a success message and the
//! message's contents. If a log file is configured, the message will also have
//! the timestamp information when written to the log file.
//!
//! Usage example:
//! \code
//! int arg = 0;
//! success("Example", "%d: This is a success message with a context.\n", arg);
//! success(NULL, "%d: This is also a success message.\n", arg+1);
//! \endcode
void success(const char *context, const char *format, ...) {

  const char msg_type[10] = "(Success)";
  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  color_text(logger_color_pallet.tag_colors[SUCCESS_TAG].text_color);
  color_background(
    logger_color_pallet.tag_colors[SUCCESS_TAG].background_color
  );
  printf("%s ", msg_type);

  // Print message contents:
  color_text(logger_color_pallet.message_colors[SUCCESS_MSG].text_color);
  color_background(
    logger_color_pallet.message_colors[SUCCESS_MSG].background_color
  );
  print_formatted_text(format, arg_list);

  // Reset display colors:
  reset_colors();

  // If a log file exists, write the message contents to it:
  if(log_file != NULL)
    log_message(log_file, time_fmt, context, msg_type, format, arg_list);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

//! \fn void unlock_logger_recursive_mutex()
//! \brief Unlock the Message Logger's recursive mutex lock, allowing other
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
//! \b Important: For this function to work properly, the function
//! enable_thread_safety() must have been called to configure the recursive
//! mutex lock. Otherwise, this function will emit a warning and will have NO
//! EFFECT.
//!
//! Usage example:
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
void unlock_logger_recursive_mutex() {
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  else
    warning(
      "Logger module",
      "Enable thread safety to access the logger recursive mutex."
    );
}

//! \fn void warning(const char *context, const char *format, ...)
//! \brief Log a warning message using the Message Logger.
//! \param context Caller context where message originated. Pass a NULL pointer
//! for an empty context.
//! \param format String formatting for the message's contents before argument
//! substitution takes place.
//! \param ... Arguments used to substitute placeholders in the message's
//! contents.
//!
//! This function logs a warning message to the terminal and configured log
//! file. The message contains the caller context that originated the warning,
//! a tag that identifies the message as a warning and the message's contents.
//! If a log file is configured, the message will also have the timestamp
//! information when written to the log file.
//!
//! Usage example:
//! \code
//! int arg = 0;
//! warning("Example", "%d: This is a warning message with a context.\n", arg);
//! warning(NULL, "%d: This is also a warning message.\n", arg+1);
//! \endcode
void warning(const char *context, const char *format, ...) {

  const char msg_type[10] = "(Warning)";
  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  color_text(logger_color_pallet.tag_colors[WARNING_TAG].text_color);
  color_background(
    logger_color_pallet.tag_colors[WARNING_TAG].background_color
  );
  printf("%s ", msg_type);

  // Print message contents:
  color_text(logger_color_pallet.message_colors[WARNING_MSG].text_color);
  color_background(
    logger_color_pallet.message_colors[WARNING_MSG].background_color
  );
  print_formatted_text(format, arg_list);

  // Reset display colors:
  reset_colors();

  // If a log file exists, write the message contents to it:
  if(log_file != NULL)
    log_message(log_file, time_fmt, context, msg_type, format, arg_list);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

// Private function implementations:
static void apply_all_default_attributes() {
  printf("\x1B[0m");
}

static void clear_background_in_current_line() {
  // When bash creates a new line, it colors the background of the entire new
  // line automatically. The following printf clears any existing background
  // on the current line.
  printf("\x1B[K");
}

static void copy_display_colors(
  DisplayColors* destination,
  const DisplayColors* origin
) {
  destination->background_color = origin->background_color;
  destination->text_color = origin->text_color;
}

static void log_datetime(FILE* log_file, const char* str_format) {

  char time_string[40];
  struct tm *time_info;
  time_t raw_time;

  // Get time and date info:
  time(&raw_time);
  time_info = localtime(&raw_time);

  // Format it into a string:
  strftime(time_string, 40, str_format, time_info);

  // Log the string to a file:
  fprintf(log_file, "[%s] ", time_string);

}

static void log_formatted_text_content(
  FILE* log_file,
  const char* text_format,
  va_list text_args
) {

  va_list text_args_copy;

  // We need to copy the args va_list because any v*printf() makes the va_list
  // unusable for future v*printf() calls.
  va_copy(text_args_copy, text_args);
  vfprintf(log_file, text_format, text_args_copy);
  va_end(text_args_copy);

}

static void log_message(
  FILE* log_file,
  const char* date_time_format,
  const char* msg_context,
  const char* msg_type,
  const char* msg_format,
  va_list msg_args
) {

  // This check is a little of an overkill... but better safe than sorry!
  if(log_file != NULL) {

    // Log the date and time according to the format specified by the user:
    log_datetime(log_file, date_time_format);

    // Log the message context:
    if(msg_context != NULL)
      fprintf(log_file, "%s: ", msg_context);

    // Log the message type:
    if(msg_type != NULL)
      fprintf(log_file, "%s ", msg_type);

    log_formatted_text_content(log_file, msg_format, msg_args);

  }
}

static void print_context(const char *context) {
  color_text(logger_color_pallet.tag_colors[CONTEXT_TAG].text_color);
  color_background(
    logger_color_pallet.tag_colors[CONTEXT_TAG].background_color
  );
  printf("%s: ", context);
}

static void print_formatted_text(const char* text_format, va_list text_args) {

  va_list text_args_copy;

  // We need to copy the args va_list because any v*printf() makes the va_list
  // unusable for future v*printf() calls.
  va_copy(text_args_copy, text_args);
  vprintf(text_format, text_args_copy);
  va_end(text_args_copy);

}
