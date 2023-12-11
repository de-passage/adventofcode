# Specify the compiler to use
CC = clang++

# Specify the target executable
TARGET = $(basename $(notdir $(wildcard *.cpp)))

# Specify the source files
SOURCES = $(wildcard *.cpp)

# Specify the object files
OBJECTS = $(SOURCES:.cpp=.o)

# Specify the include directories
INCLUDES =

# Specify the library directories
LIBRARIES =

# Specify the libraries to link with
LIBS =

# Specify the compile flags
CFLAGS =

ACTUAL_CFLAGS = $(CFLAGS) $(shell cat compile_flags.txt)

# Specify the linker flags
LDFLAGS =

.PHONY: all clean run_test
run_test: test
	./test
	@echo "Test passed successfully!"

test: test.cpp
	$(CC) $(REAL_CFLAGS) $(LDFLAGS) $< -o $@ $(LIBS) -g

# Target for the executable
%: %.cpp
		$(CC) $(ACTUAL_CFLAGS) $(LDFLAGS) $< -o $@ $(LIBS)

# Target for cleaning up
clean:
		rm -f $(OBJECTS) $(TARGET)
