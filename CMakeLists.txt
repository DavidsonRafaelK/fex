cmake_minimum_required(VERSION 3.16)
project(fex C)

set(PROJET_VERSION "0.1.0")
set(PROJET_DESCRIPTION "FEX is a fast and efficient file explorer for Linux. Build from scratch with a focus on performance and simplicity.")

set(CMAKE_C_STANDARD 11)
SET(cmake_C_STANDARD_REQUIRED ON)
SET(CMAKE_C_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-g -00 -DDEBUG -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -flto")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${X11_INCLUDE_DIR}
    ${CAIRO_INCLUDE_DIRS}
    ${PANGO_INCLUDE_DIRS}
    ${FONTCONFIG_INCLUDE_DIRS}
)

set(COMMON_SOURCES
    src/common.c
)

set(WINDOW_SOURCES
    src/window.c
)

set(FILE_SOURCES
    src/file_ops.c
)

set(RENDER_SOURCES
    src/render.c
)

set(INPUT_SOURCES
    src/input.c
)

set(MAIN_SOURCES
    src/main.c
)

set(ALL_SOURCES
    ${COMMON_SOURCES}
    ${WINDOW_SOURCES}
    ${FILE_SOURCES}
    ${RENDER_SOURCES}
    ${INPUT_SOURCES}
    ${MAIN_SOURCES}
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
    ${EXTRA_LIBS}
    m  # Math library
)

configure_file(
    "${CMAKE_SOURCE_DIR}/config.h.in"
    "${CMAKE_BINARY_DIR}/config.h"
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}")

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "DavidsonRafaelK")
set(CPACK_PACKAGE_CONTACT "davidsonrafael20@gmail.com")

option(ENABLE_TESTS "Enable testing" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)
option(ENABLE_DOCUMENTATION "Enable documentation generation" OFF)

if(ENABLE_TESTS)
    enable_testing()
    # Add test subdirectory if it exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests")
        add_subdirectory(tests)
    endif()
endif()

if(ENABLE_STATIC_ANALYSIS)
    find_program(CLANG_TIDY clang-tidy)
    if(CLANG_TIDY)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            C_CLANG_TIDY "${CLANG_TIDY};-checks=-*,readability-*,performance-*,portability-*"
        )
    endif()
endif()

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

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C flags: ${CMAKE_C_FLAGS}")
if(HAVE_LIBMAGIC)
    message(STATUS "libmagic: Found")
else()
    message(STATUS "libmagic: Not found (file type detection will be limited)")
endif()