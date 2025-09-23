cmake_minimum_required(VERSION 3.10)

# Project name and language
project(main_project C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    src
    src/grammar
    src/tokenizer
    src/parser
    src/transpiler
)

# Add the executable
add_executable( 
   main
    src/main.c
    src/tokenizer/tokenizer.c
    src/parser/parser.c
    src/transpiler/transpiler.c
)

