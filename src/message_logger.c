// Copyright (c) 2019 André Filipe Caldas Laranjeira
// MIT License

// Message Logger module - Source code.
//! \file message_logger.c
//! \author André Filipe Caldas Laranjeira
//! \brief Message Logger module - Source code.
//!
//! The Message Logger module source file contains the implementation of all
//! the functions provided by the Message Logger module and private state
//! variables.

// Includes:
#include "message_logger.h"

// Private constants:
//! \brief Default logger color pallet configuration.
const static LoggerColorPallet default_color_pallet = {
  .message_colors = DEFAULT_LOGGER_MESSAGE_COLORS,
  .tag_colors = DEFAULT_LOGGER_TAG_COLORS
};

// Private variables:

//! \brief Message Logger's file pointer for any configured log file.
static FILE *log_file = NULL;

//! \brief Message Logger's color pallet for messages and tags.
static LoggerColorPallet logger_color_pallet = {
  .message_colors = DEFAULT_LOGGER_MESSAGE_COLORS,
  .tag_colors = DEFAULT_LOGGER_TAG_COLORS
};

//! \brief Message Logger's recursive mutex used to ensure thread safety.
static pthread_mutex_t *logger_recursive_mutex = NULL;

//! \brief Message Logger's time format for log file timestamps.
static TimeFormat logger_time_fmt = {
  .string_representation = "%H:%M:%S %d-%m-%Y"
};

// Private function prototypes:

//! \fn static void apply_all_default_attributes()
//! \brief Reset all the terminal's colors and text attributes to their
//! defaults.
//!
//! This function resets all of the terminal's colors and text attributes to
//! the default configurations specified for the terminal. This includes the
//! colors used for text font and text background, the font weight and any
//! other characteristics configured using ANSI escape codes.
//!
//! \par Usage example
//! \code
//! // Change various text configurations.
//! apply_all_default_attributes();
//! \endcode
static void apply_all_default_attributes();

//! \fn static void clear_line_text_background_past_cursor()
//! \brief Clears the existing colored text background past the cursor in the
//! current line.
//!
//! This function clears any existing colored text background past the cursor
//! position in the current line. When a line break occurs in the terminal, the
//! new line is entirely colored with the current text background color.
//! Therefore, when any change in the text background color occurs, it is
//! necessary to clear the text background in the current line past the cursor
//! position to allow the new text background color to take effect.
//!
//! \par Usage example
//! \code
//! // Color the text's background with a certain color.
//! // Color the text's background with another color.
//! clear_line_text_background_past_cursor();
//! \endcode
static void clear_line_text_background_past_cursor();

//! \fn static void copy_display_colors(
//!   DisplayColors* destination,
//!   const DisplayColors* origin
//! )
//! \brief Copies data between DisplayColors variables. Both pointers MUST NOT
//! be NULL!
//! \param destination Destination for data to be copied to. Must NOT be NULL!
//! \param origin Source for data to be copied from. Must NOT be NULL!
//!
//! This function copies data between two DisplayColors variables, duplicating
//! any and all data. This includes the data stored in pointers.
//!
//! This function exists because, while the assignment operator may be used to
//! copy the contents of a struct, that copying process doesn't duplicate the
//! contents of a pointer, instead copying the same pointer to another struct.
//! Therefore, this function should be used to avoid getting possible pointer
//! duplicates when copying DisplayColors data structures.
//!
//! \warning This functions does not check if both pointers are valid! Do NOT
//! call this function with NULL pointers.
//!
//! \par Usage example
//! \code
//! DisplayColors origin, destination;
//!
//! origin.text_color = B_GRN;
//! origin.background_color = B_WHT;
//!
//! copy_display_colors(&destination, &origin);
//! \endcode
static void copy_display_colors(
  DisplayColors* destination,
  const DisplayColors* origin
);

//! \fn static void log_formatted_text_content(
//!   FILE* log_file,
//!   const char* text_format,
//!   va_list text_args
//! )
//! \brief Writes a formatted text with it's arguments to a log file. Log file
//! pointer must NOT be NULL.
//! \param log_file Log file where text will be logged. Must NOT be NULL.
//! \param text_format String formatting for the text's contents before
//! argument substitution takes place.
//! \param text_args Arguments used to substitute placeholders in the text's
//! contents.
//!
//! This function writes a formatted text to a log file, utilizing the
//! arguments provided to substitute any placeholders in the text's initial
//! format.
//!
//! \note The va_list provided as an argument for this function is NOT rendered
//! unusable by this function! A local copy of the va_list argument is made,
//! allowing other functions to reuse it. However, this implies that the memory
//! cleaning for the va_list achieved with va_end() is a task that this
//! function does NOT handle!
//!
//! \warning This functions does not check if the file pointer is valid! Do NOT
//! call this function with a NULL file pointer.
//!
//! \par Usage example
//! \code
//! FILE *log_file;
//!
//! void question(const char* text_format, ...) {
//!   va_list text_args;
//!   va_start(text_args, text_format);
//!   log_formatted_text_content(
//!     log_file,
//!     text_format,
//!     text_args
//!   );
//!   va_end(text_args);
//! }
//!
//! int main() {
//!   int num = 3;
//!   log_file = fopen("test-logfile.log", "w");
//!   // Use the question function. The text should be logged correctly.
//!   question("Number %d: What is the mitochondria?", int);
//!   fclose(log_file);
//!   return 0;
//! }
//! \endcode
static void log_formatted_text_content(
  FILE* log_file,
  const char* text_format,
  va_list text_args
);

//! \fn static void log_message(
//!   FILE* log_file,
//!   TimeFormat* time_format,
//!   const char* msg_context,
//!   const char* msg_type,
//!   const char* msg_format,
//!   va_list msg_args
//! )
//! \brief Writes a text message with it's timestamp information to a log file.
//! Log file pointer must NOT be NULL.
//! \param log_file File where message will be logged. Must NOT be NULL.
//! \param time_format Time format used to generate timestamp in the log file.
//! \param msg_context Text containing the message's caller context. Pass a
//! NULL pointer to log a message without context.
//! \param msg_type Text that identifies the type of message logged.
//! \param msg_format Message's text format without substitution arguments.
//! \param msg_args Arguments to substitute in message's text format.
//!
//! This function writes a timestamped message to a log file. The timestamp
//! formatting and contents is determined by the time_format argument. To log a
//! message, the conventional structure of context, type and format is
//! required, as this structure is used by all default message types provided
//! by the Message Logger module.
//!
//! If we indicate the contents of a variable by "${VARIABLE}", we can state
//! that a typical message logged will appear like so:
//! \verbatim [${TIMESTAMP}] ${CONTEXT}: ${TYPE} ${MESSAGE_TEXT} \endverbatim
//!
//! An example of a logged message in the sample file appears below:
//! \verbatim [23:17:15] Main: (Success) Thread 1 finished! \endverbatim
//!
//! \note The va_list provided as an argument for this function is NOT rendered
//! unusable by this function! All functions called by this function and that
//! utilize the va_list argument should make a local copy of it, allowing other
//! functions to reuse it. However, this implies that the memory cleaning for
//! the va_list achieved with va_end() is a task that this function does NOT
//! handle!
//!
//! \warning This function should receive a log_file pointer that is NOT NULL
//! to work as expected. Otherwise, it will have NO EFFECT!
//!
//! \par Usage example
//! \code
//! FILE *log_file;
//! TimeFormat time_format = {
//!   .string_representation = "%H:%M:%S"
//! };
//!
//! void question(const char* context, const char* text_format, ...) {
//!   va_list text_args;
//!   va_start(text_args, text_format);
//!   log_message(
//!     log_file,
//!     &time_format,
//!     context,
//!     "(Question)",
//!     text_format,
//!     text_args
//!   );
//!   va_end(text_args);
//! }
//!
//! int main() {
//!   int num = 3;
//!   log_file = fopen("test-logfile.log", "w");
//!   // Use the question function. The question should be logged correctly in
//!   // the expected message format and with a timestamp.
//!   question("Biology test", "Number %d: What is the mitochondria?", int);
//!   fclose(log_file);
//!   return 0;
//! }
//! \endcode
static void log_message(
  FILE* log_file,
  TimeFormat* time_format,
  const char* msg_context,
  const char* msg_type,
  const char* msg_format,
  va_list msg_args
);

//! \fn static void log_timestamp(FILE* log_file, TimeFormat* time_format)
//! \brief Writes a timestamp to a log file with a certain format. Log file
//! pointer must NOT be NULL.
//! \param log_file File where timestamp will be logged. Must NOT be NULL.
//! \param time_format Time format used to generate timestamp.
//!
//! This function writes a timestamp to a log file. The time_format argument
//! determines the formatting and contents of the timestamp. This function is
//! generally called by functions that write message's contents to a log file
//! (e.g: the \link log_message() log_message \endlink function) to register
//! the time in which the message was generated by the Message Logger module.
//!
//! \warning This functions does not check if the file pointer is valid! Do NOT
//! call this function with a NULL file pointer.
//!
//! \par Usage example
//! \code
//! FILE *log_file;
//! TimeFormat time_format = {
//!   .string_representation = "%H:%M:%S"
//! };
//!
//! int main() {
//!   log_file = fopen("test-logfile.log", "w");
//!   log_timestamp(log_file, &time_format);
//!   fprintf(log_file, "This message is being logged and timestamped!\n");
//!   fclose(log_file);
//!   return 0;
//! }
//! \endcode
static void log_timestamp(FILE* log_file, TimeFormat* time_format);

//! \fn static void print_context(const char *context)
//! \brief Prints a message's caller context in tag format.
//! \param context Text containing the message's caller context.
//!
//! This function writes the context tag of a message to the terminal. The
//! colors used for the context tag are taken from the \link
//! #logger_color_pallet Message Logger's color pallet. \endlink
//!
//! \par Usage example
//! \code
//! print_context("Main function");
//! printf("This message came from the main function!\n");
//! \endcode
static void print_context(const char *context);

//! \fn static void print_formatted_text(
//!   const char* text_format,
//!   va_list text_args
//! )
//! \brief Prints a formatted text after argument substitution.
//! \param text_format String formatting for the text's contents before
//! argument substitution takes place.
//! \param text_args Arguments used to substitute placeholders in the text's
//! contents.
//!
//! This function writes a formatted text to the terminal, utilizing the
//! arguments provided to substitute any placeholders in the text's initial
//! format.
//!
//! \note The va_list provided as an argument for this function is NOT rendered
//! unusable by this function! A local copy of the va_list argument is made,
//! allowing other functions to reuse it. However, this implies that the memory
//! cleaning for the va_list achieved with va_end() is a task that this
//! function does NOT handle!
//!
//! \par Usage example
//! \code
//! void question(const char* text_format, ...) {
//!   va_list text_args;
//!   va_start(text_args, text_format);
//!   print_formatted_text(text_format, text_args);
//!   va_end(text_args);
//! }
//!
//! int main() {
//!   int num = 3;
//!   question("Number %d: What is the mitochondria?", int);
//!   return 0;
//! }
//! \endcode
static void print_formatted_text(const char* text_format, va_list text_args);

// Public function implementations:
int configure_log_file(const char *file_name, LogFileMode file_mode) {

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
    logger_time_fmt.string_representation,
    TIME_FMT_SIZE
  );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  return 0;

}

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
  strncpy(logger_time_fmt.string_representation, new_format, TIME_FMT_SIZE);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  return 0;

}

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

  clear_line_text_background_past_cursor();

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

}

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
    log_message(
      log_file,
      &logger_time_fmt,
      context,
      msg_type,
      format,
      arg_list
    );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

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
    log_message(
      log_file,
      &logger_time_fmt,
      context,
      msg_type,
      format,
      arg_list
    );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

void lock_logger_recursive_mutex() {
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  else
    warning(
      "Logger module",
      "Enable thread safety to access the logger recursive mutex."
    );
}

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
    log_message(
      log_file,
      &logger_time_fmt,
      context,
      NULL,
      format,
      arg_list
    );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

void reset_background_color() {
  color_background(DFLT);
}

void reset_colors() {
  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  apply_all_default_attributes();
  clear_line_text_background_past_cursor();

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);
}

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

void reset_text_color() {
  color_text(DFLT);
}

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
    log_message(
      log_file,
      &logger_time_fmt,
      context,
      msg_type,
      format,
      arg_list
    );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

void unlock_logger_recursive_mutex() {
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  else
    warning(
      "Logger module",
      "Enable thread safety to access the logger recursive mutex."
    );
}

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
    log_message(
      log_file,
      &logger_time_fmt,
      context,
      msg_type,
      format,
      arg_list
    );

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

static void clear_line_text_background_past_cursor() {
  // When bash creates a new line, it colors the background of the entire new
  // line automatically. The following printf clears any existing background
  // on the current line past the cursor position.
  printf("\x1B[K");
}

static void copy_display_colors(
  DisplayColors* destination,
  const DisplayColors* origin
) {
  destination->background_color = origin->background_color;
  destination->text_color = origin->text_color;
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
  TimeFormat* time_format,
  const char* msg_context,
  const char* msg_type,
  const char* msg_format,
  va_list msg_args
) {

  // This check is a little of an overkill... but better safe than sorry!
  if(log_file != NULL) {

    // Log the timestamp according to the format specified by the user:
    log_timestamp(log_file, time_format);

    // Log the message context:
    if(msg_context != NULL)
      fprintf(log_file, "%s: ", msg_context);

    // Log the message type:
    if(msg_type != NULL)
      fprintf(log_file, "%s ", msg_type);

    log_formatted_text_content(log_file, msg_format, msg_args);

  }
}

static void log_timestamp(FILE* log_file, TimeFormat* time_format) {

  char timestamp[TIME_FMT_SIZE];
  struct tm *time_info;
  time_t raw_time;

  // Get current time information:
  time(&raw_time);
  time_info = localtime(&raw_time);

  // Format it into a string:
  strftime(
    timestamp,
    TIME_FMT_SIZE,
    time_format->string_representation,
    time_info
  );

  // Log the string to a file:
  fprintf(log_file, "[%s] ", timestamp);

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
