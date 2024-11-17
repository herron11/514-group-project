CC = clang++
CFLAGS = -Wall -pthread
TARGET = thread_manager

all: $(TARGET)

$(TARGET): thread_manager.cpp
	$(CC) $(CFLAGS) -o $(TARGET) thread_manager.cpp

clean:
	rm -f $(TARGET)
