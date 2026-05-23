cmake_minimum_required(VERSION 3.10)
project(SortMergeJoin)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(SortMergeJoin 
    src/main.cpp 
    src/Esquema.cpp 
    src/Tupla.cpp 
    src/Pagina.cpp 
    src/Tabela.cpp 
    src/SortMergeJoin.cpp
)