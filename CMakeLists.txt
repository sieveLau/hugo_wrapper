cmake_minimum_required(VERSION 3.23)
include(FetchContent)
project(hugo_wrapper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 9.1.0
)
FetchContent_MakeAvailable(fmt)

include_directories(${fmt_SOURCE_DIR}/include)

add_executable(hugo_wrapper main.cpp)
target_link_libraries(hugo_wrapper fmt::fmt)
