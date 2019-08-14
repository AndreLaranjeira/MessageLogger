// Message logger module - Source code.

// Includes:
#include "message_logger.h"

// Private function headers:
void print_context(const char*);

// Public function implementations:
void error(const char *context, const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  printf(B_RED "[Error]" RESET);
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

void info(const char *context, const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  printf(B_BLU "[Info]" RESET);
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

void message(const char *context, const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

void success(const char *context, const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  printf(B_GRN "[Success]" RESET);
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

void warning(const char *context, const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  printf(B_YEL "[Warning]" RESET);
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

// Private function implementations:
void print_context(const char *context) {
  printf(B_WHT "%s: ", context);
  printf(RESET);
}
