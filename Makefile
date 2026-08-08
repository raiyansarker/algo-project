CXX ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall
SRC = main.cpp
TARGET ?= game
LDLIBS ?= -lncurses

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

macos-universal:
	$(CXX) $(CXXFLAGS) -arch x86_64 -arch arm64 $(SRC) -o game-macos-universal $(LDLIBS)

clean:
	rm -f game game.exe game-* *.o

.PHONY: run clean macos-universal
