cmake_minimum_required(VERSION 3.16)
project(TreasureHunter CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Attempt to locate system Curses / ncurses / PDCurses
find_package(Curses QUIET)

if(CURSES_FOUND)
    message(STATUS "Found Curses: ${CURSES_LIBRARIES}")
    add_executable(game main.cpp)
    target_include_directories(game PRIVATE ${CURSES_INCLUDE_DIRS})
    target_link_libraries(game PRIVATE ${CURSES_LIBRARIES})
else()
    message(STATUS "System Curses not found. Fetching PDCursesMod automatically...")
    include(FetchContent)
    FetchContent_Declare(
        pdcurses
        GIT_REPOSITORY https://github.com/wmcbrine/PDCursesMod.git
        GIT_TAG v4.3.7
    )
    # Build PDCursesMod static library without apps/demos
    set(PDC_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(pdcurses)

    add_executable(game main.cpp)
    target_link_libraries(game PRIVATE pdcurses)
endif()
