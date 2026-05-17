cmake_minimum_required(VERSION 3.16)
project(SortMergeJoin CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

# Coleta todos os .cpp de src/ e utils/
file(GLOB SRC_FILES   "src/*.cpp")
file(GLOB UTILS_FILES "utils/*.cpp")

add_executable(smj ${SRC_FILES} ${UTILS_FILES})

target_include_directories(smj PRIVATE include utils)
