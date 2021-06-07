// Test program for MessageLogger module.

// Includes:
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "message_logger.h"

// Macros:
#define THREAD_NUM 2

// Auxiliary function headers:
void* thread_example(void*);

// Main function:
int main() {

  // Variable declaration:
  int i, thread_args[THREAD_NUM];
  pthread_t thread_ids[THREAD_NUM];

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

  printf("\n");

  // Enabling thread safety:
  printf("Enabling thread safety: \n");

  enable_thread_safety();

  // Create threads:
  for (i = 0; i < THREAD_NUM; i++) {
    thread_args[i] = i + 1;
    pthread_create(&thread_ids[i], NULL, thread_example, &thread_args[i]);
  }

  // Join threads:
  for (i = 0; i < THREAD_NUM; i++) {
    pthread_join(thread_ids[i], NULL);
    success("Main", "Thread %d finished!\n", i+1);
  }


  // Clean up:
  logger_module_clean_up();

  printf("\n");

  return 0;

}

// Auxiliary functions:
void* thread_example (void *args) {

  // Variable declaration:
  char thread_context[20];
  int i, thread_id;

  // Configure thread context:
  thread_id = *((int*) args);
  sprintf(thread_context, "Thread %d", thread_id);

  // Log a lot of messages with different functions:
  for (i = 0; i < 15; i++) {

    switch (i % 5) {
      case 0:
        message(
          thread_context,
          "Message number %d!\n",
          i+1
        );
        break;

      case 1:
        error(
          thread_context,
          "Message number %d!\n",
          i+1
        );
        break;

      case 2:
        info(
          thread_context,
          "Message number %d!\n",
          i+1
        );
        break;

      case 3:
        success(
          thread_context,
          "Message number %d!\n",
          i+1
        );
        break;

      case 4:
        warning(
          thread_context,
          "Message number %d!\n",
          i+1
        );
        break;
    }

  }

  // Finish execution:
  pthread_exit((void *) 0);
}
