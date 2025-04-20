# Compiler
CC = g++
#CFLAGS = -g -fsanitize=address -static-libasan -fno-omit-frame-pointer -Wall -Wextra -Wpedantic
#LDFLAGS = -fsanitize=address -static-libasan

# Target Executable
TARGET = sim_cache

# Source Code
SOURCE = src/main.cpp src/FileProcessor.cpp src/Cache.cpp src/MemoryHierarchy.cpp src/OracleFileProcessor.cpp src/GraphCache.cpp

# Builds the executable with AddressSanitizer
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LDFLAGS)

# Cleans any existing files
clean:
	rm -f $(TARGET)
