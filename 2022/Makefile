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
CFLAGS = -Wall -g

# Specify the linker flags
LDFLAGS =

# Target for the executable
$(TARGET): $(TARGET).o
		$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LIBS)

# Target for object files
%.o: %.cpp
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Target for cleaning up
clean:
		rm -f $(OBJECTS) $(TARGET)
