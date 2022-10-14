cmake_minimum_required(VERSION 3.23)
include(FetchContent)
project(hugo_wrapper LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.1.0
)
FetchContent_MakeAvailable(fmt)
include_directories(${fmt_SOURCE_DIR}/include)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)
include_directories(${json_SOURCE_DIR}/include)

FetchContent_Declare(util
        GIT_REPOSITORY https://github.com/sieveLau/cpp_util.git
        GIT_TAG v0.1.0-dev
        )
FetchContent_MakeAvailable(util)
include_directories(${util_SOURCE_DIR}/include)

configure_file(config.json config.json COPYONLY)

add_executable(hugo_wrapper main.cpp)
target_link_libraries(hugo_wrapper PRIVATE fmt::fmt PRIVATE nlohmann_json::nlohmann_json path_helper)
