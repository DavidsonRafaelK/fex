cmake_minimum_required(VERSION 3.16)
project(fex C)

set(PROJECT_VERSION "0.1.0")
set(PROJECT_DESCRIPTION "FEX is a fast and efficient file explorer for Linux. Built from scratch with a focus on performance and simplicity.")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-g -O0 -DDEBUG -fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
endif()

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(X11 REQUIRED x11)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(PANGO REQUIRED pango)
pkg_check_modules(PANGOCAIRO REQUIRED pangocairo)
pkg_check_modules(FONTCONFIG REQUIRED fontconfig)

# Includes
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${X11_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
    ${PANGO_INCLUDE_DIRS}
    ${PANGOCAIRO_INCLUDE_DIRS}
    ${FONTCONFIG_INCLUDE_DIRS}
)

# Sources
set(ALL_SOURCES
    src/common.c
    src/window.c
    src/main.c
    # Add more if needed
)

add_executable(${PROJECT_NAME} ${ALL_SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic
    -Wformat=2 -Wformat-security
    -Wnull-dereference -Wstack-protector
    -Wstrict-overflow=3 -Wlogical-op
    -Wduplicated-cond -Wduplicated-branches
    -Wjump-misses-init -Wdouble-promotion
    -Wshadow -Wundef -fno-common
    ${CAIRO_CFLAGS_OTHER}
    ${PANGO_CFLAGS_OTHER}
    ${FONTCONFIG_CFLAGS_OTHER}
)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_link_options(${PROJECT_NAME} PRIVATE
        -Wl,-z,relro,-z,now
        -Wl,-z,noexecstack
        -Wl,--as-needed
    )
endif()

target_link_libraries(${PROJECT_NAME}
    ${X11_LIBRARIES}
    ${CAIRO_LIBRARIES}
    ${PANGO_LIBRARIES}
    ${PANGOCAIRO_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    m
)

configure_file(
    "${CMAKE_SOURCE_DIR}/config.h.in"
    "${CMAKE_BINARY_DIR}/config.h"
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}")

# Package Info
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "DavidsonRafaelK")
set(CPACK_PACKAGE_CONTACT "davidsonrafael20@gmail.com")

# Options
option(ENABLE_TESTS "Enable testing" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)
option(ENABLE_DOCUMENTATION "Enable documentation generation" OFF)

# Testing
if(ENABLE_TESTS)
    enable_testing()
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests")
        add_subdirectory(tests)
    endif()
endif()

# Static Analysis
if(ENABLE_STATIC_ANALYSIS)
    find_program(CLANG_TIDY clang-tidy)
    if(CLANG_TIDY)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            C_CLANG_TIDY "${CLANG_TIDY};-checks=-*,readability-*,performance-*,portability-*"
        )
    endif()
endif()

# Documentation
if(ENABLE_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
        )
    endif()
endif()

# Debug output
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C flags: ${CMAKE_C_FLAGS}")
