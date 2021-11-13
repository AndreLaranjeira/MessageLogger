// Copyright (c) 2019 André Filipe Caldas Laranjeira
// MIT License

// Test program for the Message Logger module.

// Includes:
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "message_logger.h"

// Macros:
#define THREAD_NUM 4

// Auxiliary function prototypes:
void* thread_example(void *args);

// Main function:
int main() {

  // Variable declaration:
  DisplayColors current_success_message_colors, current_success_tag_colors;
  DisplayColors custom_context_tag_colors, custom_info_msg_colors;
  DisplayColors custom_info_tag_colors;
  int i, thread_args[THREAD_NUM];
  pthread_t thread_ids[THREAD_NUM];
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

  printf("\n");

  // Using multiple threads:
  printf("Using multiple threads: \n");

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

  printf("\n");

  // Getting the display colors currently used in the logger:
  printf("Getting the display colors currently used in the logger: \n");

  get_logger_msg_colors(&current_success_message_colors, SUCCESS_MSG);
  get_logger_tag_colors(&current_success_tag_colors, SUCCESS_TAG);

  color_text(current_success_message_colors.text_color);
  color_background(current_success_tag_colors.text_color);
  printf(
    "Text and background colors copied from the success message and tag text "
    "colors!\n"
  );
  reset_colors();

  printf("\n");

  // Changing the display colors used in the logger:
  printf("Changing the display colors used in the logger: \n");

  custom_context_tag_colors.text_color = B_GRN;
  custom_context_tag_colors.background_color = B_WHT;

  custom_info_msg_colors.text_color = B_WHT;
  custom_info_msg_colors.background_color = CYN;

  custom_info_tag_colors.text_color = B_BLA;
  custom_info_tag_colors.background_color = CYN;

  set_logger_tag_colors(CONTEXT_TAG, &custom_context_tag_colors);
  set_logger_msg_colors(INFO_MSG, &custom_info_msg_colors);
  set_logger_tag_colors(INFO_TAG, &custom_info_tag_colors);

  info("My context", "This is an info message with a custom color scheme!\n");

  printf("\n");

  // Resetting the display colors used in the logger:
  printf("Resetting the display colors used in the logger: \n");

  reset_logger_colors();
  info("Another context", "The logger color scheme has been reset!\n");

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

  printf("\n");

  // Clean up:
  logger_module_clean_up();

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
  for (i = 0; i < 6; i++) {

    switch (i % 6) {
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

      case 5:
        lock_logger_recursive_mutex();
        color_text(BLU);
        color_background(B_GRN);
        printf("%s: Message number %d!\n", thread_context, i+1);
        reset_colors();
        unlock_logger_recursive_mutex();
        break;
    }

  }

  // Finish execution:
  pthread_exit((void *) 0);
}
