# Executable name:
EXE = msg-logger-sample

# Compiler name, source file extension and compilation data (flags and libs):
CC = gcc
EXT = .c
CFLAGS = -Wall -g -I $(IDIR)
LIBS = -lm

# Project paths:
IDIR = include
ODIR = src/obj
SDIR = src

# Project files:
_DEPS = message_logger.h
_OBJ = message_logger.o sample.o
_SRC = message_logger.c sample.c

# Joining file names with their respective paths:
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
SRC = $(patsubst %,$(SDIR)/%,$(_SRC))

# Object files compilation rule:
$(ODIR)/%.o: $(SDIR)/%$(EXT) $(DEPS)
	@if [ ! -d src/obj ]; then mkdir src/obj; fi
	$(CC) -c -o $@ $< $(CFLAGS)

# Project executable compilation rule:
$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# List of aditional makefile commands:
.PHONY: clean

# Command to clean object files and project executable:
clean:
	@rm -f $(ODIR)/*.o *~ core
	@if [ -f $(EXE) ]; then rm $(EXE) -i; fi
