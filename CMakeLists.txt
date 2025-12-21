cmake_minimum_required(VERSION 3.15)
project(PasswordVault VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party)

# Detect platform
if(WIN32)
    # OpenSSL Configuration for Windows
    set(OPENSSL_ROOT "C:/Program Files/OpenSSL-Win64")
    include_directories("${OPENSSL_ROOT}/include")
    link_directories("${OPENSSL_ROOT}/lib/VC/x64/MD")
    set(OPENSSL_LIBS libssl libcrypto)
    set(PLATFORM_LIBS ws2_32 crypt32)
else()
    # OpenSSL Configuration for Linux
    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})
    set(OPENSSL_LIBS OpenSSL::SSL OpenSSL::Crypto)
    set(PLATFORM_LIBS pthread)
endif()

# Find threading library
find_package(Threads REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/crypto.cpp
    src/btree.cpp
    src/auth.cpp
    src/storage.cpp
)

# Header files
set(HEADERS
    include/crypto.hpp
    include/btree.hpp
    include/auth.hpp
    include/storage.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OPENSSL_LIBS}
    Threads::Threads
    ${PLATFORM_LIBS}
)

# Test executable
set(TEST_SOURCES
    test_data_structures.cpp
    src/crypto.cpp
    src/btree.cpp
    src/auth.cpp
    src/storage.cpp
)

add_executable(TestDataStructures ${TEST_SOURCES} ${HEADERS})

target_link_libraries(TestDataStructures
    ${OPENSSL_LIBS}
    Threads::Threads
    ${PLATFORM_LIBS}
)

# Interactive test executable
set(INTERACTIVE_SOURCES
    interactive_test.cpp
    src/crypto.cpp
    src/btree.cpp
    src/auth.cpp
    src/storage.cpp
)

add_executable(InteractiveTest ${INTERACTIVE_SOURCES} ${HEADERS})

target_link_libraries(InteractiveTest
    ${OPENSSL_LIBS}
    Threads::Threads
    ${PLATFORM_LIBS}
)

# Server executable
set(SERVER_SOURCES
    src/server.cpp
    src/crypto.cpp
    src/btree.cpp
    src/auth.cpp
    src/storage.cpp
)

add_executable(password_vault_server ${SERVER_SOURCES} ${HEADERS})

target_link_libraries(password_vault_server
    ${OPENSSL_LIBS}
    Threads::Threads
    ${PLATFORM_LIBS}
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32 crypt32)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Create data directory at build time
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)

# Copy third-party headers to build directory
message(STATUS "Project will use cpp-httplib and nlohmann-json as header-only libraries")
message(STATUS "Make sure to download them to third_party/ directory:")
message(STATUS "  - third_party/httplib.h from https://github.com/yhirose/cpp-httplib")
message(STATUS "  - third_party/json.hpp from https://github.com/nlohmann/json")
