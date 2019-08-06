// Message logger module - Source code.

// Includes:
#include "message_logger.h"

// Private function headers:

// Public function implementations:
void error(const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print tags:
  printf(B_RED "[Error]" RESET);
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

void info(const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print tags:
  printf(B_BLU "[Info]" RESET);
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

void message(const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print tags:
  printf(B_WHT "[Message]" RESET);
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

void success(const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print tags:
  printf(B_GRN "[Success]" RESET);
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

void warning(const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print tags:
  printf(B_YEL "[Warning]" RESET);
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

// Private function implementations:
