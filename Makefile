# Copyright (c) 2019 André Filipe Caldas Laranjeira
# MIT License

# Message Logger module - Project Makefile.

# Executable name:
EXE = msg-logger-sample

# Project paths:
DOCDIR = doc
DOCHTMLDIR = html
DOCLATEXDIR = latex
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

# Compiler name, source file extension and compilation data (flags and libs):
CC = gcc
EXT = .c
CFLAGS = -Wall -g -I $(IDIR)
LIBS = -lm -lpthread

# Object files compilation rule:
$(ODIR)/%.o: $(SDIR)/%$(EXT) $(DEPS)
	@if [ ! -d $(ODIR) ]; then mkdir $(ODIR); fi
	$(CC) -c -o $@ $< $(CFLAGS)

# Project executable compilation rule:
$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# List of aditional makefile commands:
.PHONY: all
.PHONY: clean
.PHONY: doc

# Command to generate all targets:
all: $(EXE) doc

# Command to clean generated files:
clean:
	@rm -f $(ODIR)/*.o *~ core
	@rm -rf $(DOCDIR)
	@if [ -f $(EXE) ]; then \
		rm -i $(EXE); \
	fi

# Command to generate the documentation:
doc:
	@doxygen ./Doxyfile > /dev/null

	@echo ""
	@echo "Open documentation now?"
	@echo "h - Open HTML format"
	@echo "l - Open Latex format (pdflatex required to generate documentation)"
	@echo "n - No (DEFAULT)"

	@read -p "Choice (h/l/n): " DOC_CHOICE_RAW; \
	DOC_CHOICE=$$(echo $$DOC_CHOICE_RAW | tr '[:upper:]' '[:lower:]'); \
	if [[ $$DOC_CHOICE == "h" ]]; then \
		xdg-open $(DOCDIR)/$(DOCHTMLDIR)/index.html > /dev/null 2>&1; \
	elif [[ $$DOC_CHOICE == "l" ]]; then \
		echo ""; \
		echo "Generating latex documentation..."; \
		$(MAKE) -C $(DOCDIR)/$(DOCLATEXDIR) > /dev/null; \
		xdg-open $(DOCDIR)/$(DOCLATEXDIR)/refman.pdf > /dev/null 2>&1; \
	fi
