
cmake_minimum_required(VERSION 3.25)

project(Rooster LANGUAGES CXX)  # languageS

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)


FetchContent_MakeAvailable(fmt)

add_executable(rooster main.cpp)  # (rooster main.cpp other.cpp another.cpp)

target_link_libraries(rooster PRIVATE fmt::fmt)



