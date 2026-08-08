CXX ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall
SRC = main.cpp

ifeq ($(OS),Windows_NT)
    TARGET ?= game.exe
    LDLIBS ?= -lpdcurses
else
    TARGET ?= game
    LDLIBS ?= -lncurses
endif

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

# Cross-compilation targets
cross-linux-arm64:
	$(MAKE) CXX=aarch64-linux-gnu-g++ TARGET=game-linux-arm64 LDLIBS="-lncurses"

cross-win64:
	$(MAKE) CXX=x86_64-w64-mingw32-g++ TARGET=game-windows-x86_64.exe LDLIBS="-lpdcurses"

macos-universal:
	$(CXX) $(CXXFLAGS) -arch x86_64 -arch arm64 $(SRC) -o game-macos-universal $(LDLIBS)

clean:
	rm -f game game.exe game-* *.o

.PHONY: run clean cross-linux-arm64 cross-win64 macos-universal
