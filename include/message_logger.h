// Message logger module - Header file.

// Define guard:
#ifndef MESSAGE_LOGGER_H_
#define MESSAGE_LOGGER_H_

// Includes:
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// Macros:
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define B_RED "\x1B[1;31m"
#define B_GRN "\x1B[1;32m"
#define B_YEL "\x1B[1;33m"
#define B_BLU "\x1B[1;34m"
#define B_MAG "\x1B[1;35m"
#define B_CYN "\x1B[1;36m"
#define B_WHT "\x1B[1;37m"
#define RESET "\x1B[0m"

// Public function headers:
void error(const char*, const char*, ...);
void info(const char*, const char*, ...);
void message(const char*, const char*, ...);
void success(const char*, const char*, ...);
void warning(const char*, const char*, ...);

#endif // MESSAGE_LOGGER_H_
