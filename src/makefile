# Compiler
CC = g++

# Target Executable
TARGET = sim_cache

# Source Code
SOURCE = main.cpp FileProcessor.cpp Cache.cpp MemoryHierarchy.cpp OracleFileProcessor.cpp
# Builds the executable
$(TARGET) : $(SOURCE)
	$(CC) -o $(TARGET) $(SOURCE)

# Cleans any existing files
clean:
	rm -f $(TARGET)