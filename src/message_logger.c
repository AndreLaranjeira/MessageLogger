// Copyright (c) 2019 André Filipe Caldas Laranjeira
// MIT License

// Message logger module - Source code.

// Includes:
#include "message_logger.h"

// Private variables:
const static LoggerColorPallet default_color_pallet = {
  .message_colors = DEFAULT_LOGGER_MESSAGE_COLORS,
  .tag_colors = DEFAULT_LOGGER_TAG_COLORS
};
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
    time_fmt,
    TIME_FMT_SIZE
  );

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  return 0;

}

int set_logger_msg_colors(
  MessageCategory message_category,
  DisplayColors *assigned_colors
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
  DisplayColors *assigned_colors
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
  strncpy(time_fmt, new_format, TIME_FMT_SIZE);

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

  clear_background_in_current_line();

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
    log_message(log_file, time_fmt, context, msg_type, format, arg_list);

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
    log_message(log_file, time_fmt, context, msg_type, format, arg_list);

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
    log_message(log_file, time_fmt, context, NULL, format, arg_list);

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
  clear_background_in_current_line();

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
    log_message(log_file, time_fmt, context, msg_type, format, arg_list);

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
