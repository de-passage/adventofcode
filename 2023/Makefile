# Specify the compiler to use
CC = g++

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
CFLAGS = -Wall -g -std=c++17 $(INCLUDES)

# Specify the linker flags
LDFLAGS =

# Target for the executable
%: %.cpp
		$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LIBS)

# Target for cleaning up
clean:
		rm -f $(OBJECTS) $(TARGET)
