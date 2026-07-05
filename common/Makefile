# Makefile for common directory
# Chikwanda Chisha
# October 25, 2024

# Compiler and flags
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libengine

# Output library
LIBRARY = common.a
OBJS = pagedir.o

# Default target
$(LIBRARY): $(OBJS)
	ar cr $(LIBRARY) $(OBJS)

# Compile .c files to .o files
pagedir.o: pagedir.c pagedir.h

# Clean up
clean:
	rm -f $(OBJS) $(LIBRARY)
