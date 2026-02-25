cmake_minimum_required(VERSION 3.28)
project(PhotoGalleryBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(MSVC)
    add_compile_options(/std:c++latest /W4)
else()
    add_compile_options(-std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
endif()

# Find packages
find_package(Drogon REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(exiv2 REQUIRED)
find_package(WebP REQUIRED)
pkg_check_modules(ARGON2 REQUIRED libargon2)

# JWT-CPP
set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JWT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JWT_EXTERNAL_NLOHMANN_JSON ON CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG        v0.7.0
)
FetchContent_MakeAvailable(jwt-cpp)

# Collect all source files
file(GLOB_RECURSE SRC_FILES 
    src/api/*.cpp
    src/core/*.cpp
    src/domain/*.cpp
    src/infra/*.cpp
)

# Main Executable
add_executable(${PROJECT_NAME} src/app/main.cpp ${SRC_FILES})

# Target Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    include
)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${Drogon_INCLUDE_DIRS}
    ${jwt-cpp_SOURCE_DIR}/include
    ${ARGON2_INCLUDE_DIRS}
)

# Target Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Drogon::Drogon
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
    PostgreSQL::PostgreSQL
    jwt-cpp::jwt-cpp
    exiv2lib
    WebP::webp
    ${ARGON2_LIBRARIES}
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MAGICK Magick++-6.Q16 REQUIRED)

# Import Pipeline CLI Tool
add_executable(gallery-import src/app/import_main.cpp ${SRC_FILES})
target_include_directories(gallery-import PRIVATE 
    src 
    include
)
target_include_directories(gallery-import SYSTEM PRIVATE
    ${Drogon_INCLUDE_DIRS}
    ${MAGICK_INCLUDE_DIRS}
    ${jwt-cpp_SOURCE_DIR}/include
    ${ARGON2_INCLUDE_DIRS}
)
target_link_libraries(gallery-import PRIVATE
    Drogon::Drogon
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    PostgreSQL::PostgreSQL
    jwt-cpp::jwt-cpp
    exiv2lib
    WebP::webp
    ${MAGICK_LIBRARIES}
    ${ARGON2_LIBRARIES}
    uuid
)
