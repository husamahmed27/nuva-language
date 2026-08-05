CC = gcc
CFLAGS = -Wall -Wextra -g -Isrc
LDFLAGS =

FLEX = win_flex
BISON = win_bison

TARGET = nuva.exe

SRCS = src/main.c src/ast.c src/interpreter.c src/lexer.c src/parser.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

src/parser.c src/parser.h: src/parser.y
	$(BISON) -d -o src/parser.c src/parser.y

src/lexer.c: src/lexer.l src/parser.h
	$(FLEX) -o src/lexer.c src/lexer.l

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	@echo Running tests...
	@for %%f in (examples\*.nv) do (echo Testing %%f && echo 1 2 3 4 5 6 7 8 9 10 | .\$(TARGET) %%f)
	@echo All tests completed.

clean:
	del /Q src\lexer.c src\parser.c src\parser.h src\*.o $(TARGET) 2>nul || true

.PHONY: all clean test
