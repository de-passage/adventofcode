# Specify the compiler to use
CXX ?= clang++

# Specify the target executable
TARGET = $(basename $(notdir $(wildcard *.cpp)))

# Specify the source files
SOURCES = $(wildcard *.cpp)

# Specify the object files
OBJECTS = $(SOURCES:.cpp=.o)

# Specify the include directories
INCLUDES = -I../common

# Specify the library directories
LIBRARIES =

# Specify the libraries to link with
LIBS =

# Specify the compile flags
CFLAGS =

ACTUAL_CFLAGS = $(CFLAGS) $(shell cat compile_flags.txt) $(INCLUDES)

# Specify the linker flags
LDFLAGS =

.PHONY: all clean run_test
run_test: test
	./test
	@echo "Test passed successfully!"

test: test.cpp
	$(CXX) $(ACTUAL_CFLAGS) $(LDFLAGS) $< -o $@ $(LIBS) -g

# Target for the executable
%: %.cpp
		$(CXX) $(ACTUAL_CFLAGS) $(LDFLAGS) $< -o $@ $(LIBS)

# Target for cleaning up
clean:
		rm -f $(OBJECTS) $(TARGET)
