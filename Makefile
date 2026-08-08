CXX ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall
SRC = main.cpp

ifeq ($(OS),Windows_NT)
    TARGET ?= game.exe
    PDC_CFLAGS := $(shell pkg-config --cflags pdcurses 2>/dev/null)
    ifeq ($(PDC_CFLAGS),)
        PDC_CFLAGS = -DPDC_STATIC_BUILD
    else
        PDC_CFLAGS += -DPDC_STATIC_BUILD
    endif
    CXXFLAGS += $(PDC_CFLAGS)
    PDC_LIBS := $(shell pkg-config --libs pdcurses 2>/dev/null)
    ifeq ($(PDC_LIBS),)
        PDC_LIBS = -lpdcurses
    endif
    LDLIBS ?= $(PDC_LIBS)
else
    TARGET ?= game
    LDLIBS ?= -lncurses
endif

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

# Cross-compilation / multi-arch targets
cross-win64:
	$(MAKE) CXX=x86_64-w64-mingw32-g++ TARGET=game-windows-x86_64.exe CXXFLAGS="$(CXXFLAGS) -DPDC_STATIC_BUILD" LDLIBS="-lpdcurses"

macos-universal:
	$(CXX) $(CXXFLAGS) -arch x86_64 -arch arm64 $(SRC) -o game-macos-universal $(LDLIBS)

clean:
	rm -f game game.exe game-* *.o

.PHONY: run clean cross-win64 macos-universal
