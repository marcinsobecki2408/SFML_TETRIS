cmake_minimum_required(VERSION 3.22)
project(SFML_TETRIS)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(SFML_TETRIS
    GameFieldManager.cpp
    GameFieldManager.h
    Generator.cpp
    Generator.h
    SFML_TETRIS.cpp
    Tetromino.cpp
    Tetromino.h
    Checkpoint.cpp
    Checkpoint.h
    connector.cpp
    connector.h)

target_link_libraries(SFML_TETRIS
    sfml-graphics
    sfml-system
    sfml-window
)
