# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Isrc
LDFLAGS =

# Platform-specific tool configuration
ifeq ($(OS),Windows_NT)
    FLEX = win_flex
    BISON = win_bison
    TARGET = nuva.exe
    RM = del /Q /F
    # Windows CMD loop
    TEST_CMD = for %%f in (examples\*.nv) do (echo Testing %%f & .\$(TARGET) %%f)
    CLEAN_OBJS = src\lexer.c src\parser.c src\parser.h src\*.o
else
    FLEX = flex
    BISON = bison
    TARGET = nuva
    RM = rm -f
    # POSIX shell loop
    TEST_CMD = for file in examples/*.nv; do echo "Testing $$file..."; ./$(TARGET) "$$file"; done
    CLEAN_OBJS = src/lexer.c src/parser.c src/parser.h src/*.o
endif

# Source Files
SRCS = src/main.c src/ast.c src/interpreter.c src/lexer.c src/parser.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Bison target (Parser)
src/parser.c src/parser.h: src/parser.y
	$(BISON) -d -o src/parser.c src/parser.y

# Flex target (Lexer)
src/lexer.c: src/lexer.l src/parser.h
	$(FLEX) -o src/lexer.c src/lexer.l

# Build the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile C source files into object files
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test target: Loop through all .nv files in examples/ and execute them
test: $(TARGET)
	@echo "Running tests..."
	@$(TEST_CMD)
	@echo "All tests completed."

# Clean target: Remove generated objects, C files from flex/bison, and the executable
clean:
	-$(RM) $(CLEAN_OBJS) $(TARGET) 2>nul || true

.PHONY: all clean test
