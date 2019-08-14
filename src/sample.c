// Test program for MessageLogger module.

// Includes:
#include <stdio.h>
#include <stdlib.h>

#include "message_logger.h"

// Main function:
int main() {

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

  return 0;

}
