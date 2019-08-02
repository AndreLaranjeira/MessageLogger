// Test program for MessageLogger module.

// Includes:
#include <stdio.h>
#include <stdlib.h>

#include "message_logger.h"

// Main function:
int main() {

  float pi = 3.14;
  int a = 42;
  char g = 'a';
  char* s = "stop\0";

  // Basic functionality:
  printf("Basic message types: \n");

  success("My lucky number is %d.\n", a);
  warning("Variable g has value %c.\n", g);
  error("Return message was %s.\n", s);
  info("The value of pi is: %f.\n", pi);
  message("Test message number %d letter %c string %s.\n", a, g, s);

  printf("\n");

  return 0;

}
