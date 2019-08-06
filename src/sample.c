// Test program for MessageLogger module.

// Includes:
#include <stdio.h>
#include <stdlib.h>

#include "message_logger.h"

// Main function:
int main() {

  // Basic functionality:
  printf("Basic message types: \n");

  message("This is a normal message.\n");
  success("This is a success message.\n");
  warning("This is a warning message.\n");
  error("This is an error message.\n");
  info("This is an info message.\n");

  printf("\n");

  return 0;

}
