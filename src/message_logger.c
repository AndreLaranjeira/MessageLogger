// Message logger module - Source code.

// Includes:
#include "message_logger.h"

// Private variables:
char time_fmt[40] = "%H:%M:%S %d-%m-%Y";
FILE *log_file = NULL;
pthread_mutex_t *logger_recursive_mutex = NULL;

// Private function headers:
static void log_message(FILE*, const char*, const char*, const char*,
                        const char*, va_list);
static void log_datetime(FILE*, const char*);
static void print_context(const char*);

// Public function implementations:
int configure_log_file(const char *file_name , LogFileMode file_mode) {

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
        warning("Logger module",
                "Could not find log file! Defaulting to write mode!");
        log_file = fopen(file_name, "w");
      }

      break;

    case WRITE:
      log_file = fopen(file_name, "w");
      break;

  }

  if(log_file == NULL) {
    error(
      "Logger module",
      "Could not create log file! Please check your system."
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

void color_background(Color p_color) {

  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  switch(p_color) {

    case BLA:
      printf("\x1B[40m");
      break;

    case RED:
      printf("\x1B[41m");
      break;

    case GRN:
      printf("\x1B[42m");
      break;

    case YEL:
      printf("\x1B[43m");
      break;

    case BLU:
      printf("\x1B[44m");
      break;

    case MAG:
      printf("\x1B[45m");
      break;

    case CYN:
      printf("\x1B[46m");
      break;

    case WHT:
      printf("\x1B[47m");
      break;

    case B_BLA:
      printf("\x1B[1;40m");
      break;

    case B_RED:
      printf("\x1B[1;41m");
      break;

    case B_GRN:
      printf("\x1B[1;42m");
      break;

    case B_YEL:
      printf("\x1B[1;43m");
      break;

    case B_BLU:
      printf("\x1B[1;44m");
      break;

    case B_MAG:
      printf("\x1B[1;45m");
      break;

    case B_CYN:
      printf("\x1B[1;46m");
      break;

    case B_WHT:
      printf("\x1B[1;47m");
      break;

  }

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
      printf("\x1B[30m");
      break;

    case RED:
      printf("\x1B[31m");
      break;

    case GRN:
      printf("\x1B[32m");
      break;

    case YEL:
      printf("\x1B[33m");
      break;

    case BLU:
      printf("\x1B[34m");
      break;

    case MAG:
      printf("\x1B[35m");
      break;

    case CYN:
      printf("\x1B[36m");
      break;

    case WHT:
      printf("\x1B[37m");
      break;

    case B_BLA:
      printf("\x1B[1;30m");
      break;

    case B_RED:
      printf("\x1B[1;31m");
      break;

    case B_GRN:
      printf("\x1B[1;32m");
      break;

    case B_YEL:
      printf("\x1B[1;33m");
      break;

    case B_BLU:
      printf("\x1B[1;34m");
      break;

    case B_MAG:
      printf("\x1B[1;35m");
      break;

    case B_CYN:
      printf("\x1B[1;36m");
      break;

    case B_WHT:
      printf("\x1B[1;37m");
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
  color_text(B_RED);
  printf("%s ", msg_type);
  reset_colors();

  // Print message contents:
  vprintf(format, arg_list);

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
  color_text(B_BLU);
  printf("%s ", msg_type);
  reset_colors();

  // Print message contents:
  vprintf(format, arg_list);

  // If a log file exists, write the message contents to it:
  if(log_file != NULL)
    log_message(log_file, time_fmt, context, msg_type, format, arg_list);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

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
  vprintf(format, arg_list);

  // If a log file exists, write the message contents to it:
  if(log_file != NULL)
    log_message(log_file, time_fmt, context, NULL, format, arg_list);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

}

void reset_colors() {
  // Acquire logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_lock(logger_recursive_mutex);

  // Reset text and background colors:
  printf("\x1B[0m");

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);
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
  color_text(B_GRN);
  printf("%s ", msg_type);
  reset_colors();

  // Print message contents:
  vprintf(format, arg_list);

  // If a log file exists, write the message contents to it:
  if(log_file != NULL)
    log_message(log_file, time_fmt, context, msg_type, format, arg_list);

  // Release logger recursive lock if thread safety is enabled:
  if(logger_recursive_mutex != NULL)
    pthread_mutex_unlock(logger_recursive_mutex);

  // Free allocated resources:
  va_end(arg_list);

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
  color_text(B_YEL);
  printf("%s ", msg_type);
  reset_colors();

  // Print message contents:
  vprintf(format, arg_list);

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
static void log_message(FILE* log_file,
                        const char* date_time_format,
                        const char* msg_context,
                        const char* msg_type,
                        const char* msg_format,
                        va_list msg_args) {

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

    // And finally, log the message contents:
    vfprintf(log_file, msg_format, msg_args);

  }
}

static void log_datetime(FILE* log_file, const char* str_format) {

  char time_string[40];
  struct tm *time_info;
  time_t raw_time;

  // Ok, THIS check is an overkill! But better safe than sorry!
  if(log_file != NULL) {

    // Get time and date info:
    time(&raw_time);
    time_info = localtime(&raw_time);

    // Format it into a string:
    strftime(time_string, 40, str_format, time_info);

    // Log the string to a file:
    fprintf(log_file, "[%s] ", time_string);

  }

}

static void print_context(const char *context) {
  color_text(B_WHT);
  printf("%s: ", context);
  reset_colors();
}
