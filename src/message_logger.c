// Message logger module - Source code.

// Includes:
#include "message_logger.h"

// Private function headers:
void print_context(const char*);

// Public function implementations:
void color_background(color p_color) {

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

}

void color_text(color p_color) {

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

}

void error(const char *context, const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  color_text(B_RED);
  printf("[Error]");
  reset_colors();
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
  color_text(B_BLU);
  printf("[Info]");
  reset_colors();
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

void reset_colors() {
  printf("\x1B[0m");
}

void success(const char *context, const char *format, ...) {

  va_list arg_list;

  // Start the argument list with any arguments after the format string:
  va_start(arg_list, format);

  // Print context:
  if(context != NULL)
    print_context(context);

  // Print tags:
  color_text(B_GRN);
  printf("[Success]");
  reset_colors();
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
  color_text(B_YEL);
  printf("[Warning]");
  reset_colors();
  printf(" ");

  // Print message contents:
  vprintf(format, arg_list);

  // Free allocated memory:
  va_end(arg_list);

}

// Private function implementations:
void print_context(const char *context) {
  color_text(B_WHT);
  printf("%s: ", context);
  reset_colors();
}
