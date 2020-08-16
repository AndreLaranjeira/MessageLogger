// Test program for MessageLogger module.

// Includes:
#include <stdio.h>
#include <stdlib.h>

#include "message_logger.h"

// Main function:
int main() {

  // Variable declaration.
  TimeFormat my_time_format;

  // Basic functionality:
  printf("Basic message types: \n");

  message(NULL, "This is a normal message.\n");
  success(NULL, "This is a success message.\n");
  warning(NULL, "This is a warning message.\n");
  error(NULL, "This is an error message.\n");
  info(NULL, "This is an info message.\n");

  printf("\n");

  // Messages with context:
  printf("Messages with context: \n");

  message("Context 1", "This is a normal message with a context.\n");
  success("Context 2", "This is a success message with a context.\n");
  warning("Context 3", "This is a warning message with a context.\n");
  error("Context 4", "This is an error message with a context.\n");
  info("Context 5", "This is an info message with a context.\n");

  printf("\n");

  // Using a log file:
  printf("Creating a log file: \n");

  configure_log_file("logger-test.log", WRITE);

  message("Log context 1", "This is a normal message that is being logged.\n");
  success("Log context 2", "This is a success message that is being logged.\n");

  logger_module_clean_up();

  printf("\n");

  printf("Append to an existing log file: \n");

  configure_log_file("logger-test.log", APPEND);

  success("New context", "Appended successfully.\n");

  logger_module_clean_up();

  printf("\n");

  // Getting the current time format:
  printf("Getting the current time format: \n");

  get_time_format(&my_time_format);

  printf("Current time format: %s\n", my_time_format.string_representation);

  printf("\n");

  // Changing the time format of log messages:
  printf("Changing the time format in the log file: \n");

  configure_log_file("logger-test.log", APPEND);

  if(set_time_format("New format: %c") == 0)
    success("New time format", "Look at the log file time!\n");

  logger_module_clean_up();

  printf("\n");

  return 0;

}
