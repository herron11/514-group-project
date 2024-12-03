CXX = clang++
CXXFLAGS = -Wall -std=c++17 -pthread

SRCS = thread_manager.cpp

TARGET = thread_manager

build: $(SRCS)
	$(CXX) -o $(TARGET) $(SRCS) $(CXXFLAGS)

debug: $(SRCS)
	$(CXX) -o $(TARGET) $(SRCS) $(CXXFLAGS) -g

clean:
	rm -f $(TARGET)

install: build
	@echo "Installing $(TARGET) to /usr/local/bin"
	cp $(TARGET) /usr/local/bin
