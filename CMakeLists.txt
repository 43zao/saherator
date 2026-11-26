cmake_minimum_required(VERSION 3.10)

project(Saherator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES
    src/*.cpp
)

add_executable(Saherator ${SOURCES})

# Fathom provides Syzygy tablebase probing. The tablebase data stays external.
target_sources(Saherator PRIVATE third_party/fathom/tbprobe.c)
target_include_directories(Saherator PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/fathom")

# Optionally include a Polyglot book in the executable so it is not needed at
# runtime. The UCI BookFile option can still select a separate external book.
option(SAHERATOR_EMBED_OPENING_BOOK "Embed a Polyglot opening book in the executable" ON)
set(SAHERATOR_OPENING_BOOK_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/opening-book/komodo.bin"
    CACHE FILEPATH "Polyglot opening book to embed")

if(SAHERATOR_EMBED_OPENING_BOOK)
    if(NOT EXISTS "${SAHERATOR_OPENING_BOOK_FILE}")
        message(FATAL_ERROR
            "SAHERATOR_EMBED_OPENING_BOOK is enabled, but the book file does not exist: ${SAHERATOR_OPENING_BOOK_FILE}")
    endif()

    file(SIZE "${SAHERATOR_OPENING_BOOK_FILE}" SAHERATOR_BOOK_SIZE)
    math(EXPR SAHERATOR_BOOK_REMAINDER "${SAHERATOR_BOOK_SIZE} % 16")
    if(SAHERATOR_BOOK_SIZE EQUAL 0 OR NOT SAHERATOR_BOOK_REMAINDER EQUAL 0)
        message(FATAL_ERROR "The selected Polyglot book must contain complete 16-byte records")
    endif()

    file(READ "${SAHERATOR_OPENING_BOOK_FILE}" SAHERATOR_BOOK_HEX HEX)
    string(REGEX REPLACE "([0-9A-Fa-f][0-9A-Fa-f])" "0x\\1," SAHERATOR_BOOK_BYTES "${SAHERATOR_BOOK_HEX}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/embedded_book_data.h"
        "#pragma once\n"
        "#include <cstddef>\n"
        "namespace EmbeddedBookData {\n"
        "inline constexpr unsigned char bytes[] = {${SAHERATOR_BOOK_BYTES}};\n"
        "inline constexpr std::size_t size = sizeof(bytes);\n"
        "}\n")

    target_include_directories(Saherator PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    target_compile_definitions(Saherator PRIVATE SAHERATOR_EMBED_OPENING_BOOK=1)
endif()

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(MSVC)

    target_compile_options(Saherator PRIVATE
        /W4
    )

    # Release optimizations
    target_compile_options(Saherator PRIVATE
        $<$<CONFIG:Release>:/O2>
    )

else()

    # Common flags
    target_compile_options(Saherator PRIVATE
        -Wall
        -Wextra
    )

    # Default optimization level
    target_compile_options(Saherator PRIVATE
        $<$<CONFIG:Debug>:-O2>
    )

    # Strong release build
    target_compile_options(Saherator PRIVATE
        $<$<CONFIG:Release>:
            -O3
            -march=native
            -flto
        >
    )

    target_link_options(Saherator PRIVATE
        $<$<CONFIG:Release>:-flto>
    )

endif()
