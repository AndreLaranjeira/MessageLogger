# MessageLogger

## Description
A simple message logger for the C programming language with different message types. Meant to be used in everyday projects to simplify the task of printing messages to the user.

## Features
- [x] Five different types of messages with context fields: message, success, warning, error and info.
- [x] Thread-safe message logging.
- [x] Public functions to color the text and the background, giving the programmer greater flexibility.
- [x] Simple way to configure a log file.

## Sample file
If you want to see this message logger in action, we have included the `sample.c` source code file, which showcases some features of the message logger developed, and a simple `Makefile` to allow the compilation of said source file into an executable.

To sample the message logger, type the following commands on a *terminal* in the **project's root directory**:
1. `make`
2. `./msg-logger-sample`
