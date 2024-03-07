# Define compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Define targets
TARGET = tittytat
SRC = tittytat.c
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Rule for building the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Clean target for removing compiled objects and executable
clean:
	rm -f $(OBJ) $(TARGET)

# Rule for installing the program (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Rule for uninstalling the program (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall
