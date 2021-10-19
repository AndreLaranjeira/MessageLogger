# Message Logger

## Description
A simple message logger for the C programming language with different message types. Meant to be used in everyday projects to simplify the task of printing messages to the user or saving them in a log file.

## Features
- Five different types of messages with context fields: message, success, warning, error and info.
- Public functions to color the text and the background, giving the programmer greater flexibility.
- Optional configuration to store logged messages in a separate log file.
  - Configurable time format for log file.
- Thread-safe message logging.
- Color customization for message types.

## How to use

### Dependencies

* `doxygen` - For generating documentation;
* `gcc` - For compiling the sample file;
* `libpthread` - For compiling the Message Logger module;
* `pdflatex` \(optional\) - For compiling the documentation in the latex format;

### Use it in your projects

To use this message logger as a module in your project, follow the steps bellow:

1. Copy the header file (`message_logger.h`) to your include directory and the source file (`message_logger.c`) to your source directory;
2. Update any Makefiles, compilation instructions or other project settings to account for these files and to use the pthreads library in compilation with the flag `-lpthread`;
3. Include the header file in your code with the command `#include "message_logger.h"`.

Feel free to use, modify and examine the message logger's code in any way that is _in accordance with the project's MIT license_.

### Generating documentation

To generate the documentation for the Message Logger module, you will need to have the program `doxygen` installed and included in your $PATH. You will also need the program `pdflatex` installed and included in your $PATH if you want to compile the documentation in the latex format.

Once you have installed the necessary dependencies, simply run the command `make doc`, on a shell from the **project's root directory**. This should generate documentation files both in the HTML and Latex formats. The makefile will then prompt you if you want to immediately open any of the generated documentation.

To open the generated HTML documentation, open the file `doc/html/index.html`. To open the generated latex documentation, compile the latex files with the generated makefile by running `make` in the directory `doc/latex` and open the file `doc/latex/refman.pdf`.

### Sample file

If you want to see this Message Logger module in action, we have included the `sample.c` source code file, which showcases some of the available features in this Message Logger module. To compile this sample into an executable, you will need the program `gcc` installed and included in your $PATH. If you wish to use a different compiler, change the `CC` variable in the `Makefile` and make any appropriate changes to how this variable is called.

To sample the Message Logger module, follow the commands bellow:

1. Run the command `make`, on a shell from the **project's root directory** to compile said source file into an executable;
2. Open the executable `msg-logger-sample` that was generated.

### Cleaning up

To clean up any object files, executables and documentation files, run the command `make clean`, on a shell from the **project's root directory**.
