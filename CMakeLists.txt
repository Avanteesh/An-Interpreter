cmake_minimum_required(VERSION 3.10)
project(main_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use llvm-config to get flags
find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config REQUIRED)

# Get LLVM include and lib info
execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --cflags
    OUTPUT_VARIABLE LLVM_CFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags --libs core native
    OUTPUT_VARIABLE LLVM_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Apply the LLVM CFLAGS
separate_arguments(LLVM_CFLAGS_LIST UNIX_COMMAND "${LLVM_CFLAGS}")
add_compile_options(${LLVM_CFLAGS_LIST})

# Your source files
set(SOURCES
    src/main.c
    src/tokenizer/tokenizer.c
    src/parser/parser.c
    src/transpiler/transpiler.c
)

# Include paths
include_directories(
    src
    src/grammar
    src/tokenizer
    src/parser
    src/transpiler
)

# Add the executable
add_executable(main ${SOURCES})

# Apply the LLVM LDFLAGS
separate_arguments(LLVM_LDFLAGS_LIST UNIX_COMMAND "${LLVM_LDFLAGS}")
target_link_libraries(main PRIVATE ${LLVM_LDFLAGS_LIST})

