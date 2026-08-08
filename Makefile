CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
LDLIBS = -lncurses
TARGET = game
SRC = main.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean