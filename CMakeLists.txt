# ------------------------- Project settings --------------------------------
project(blockchain-dvf LANGUAGES CXX)
cmake_minimum_required(VERSION 3.5.1)


set(CMAKE_CXX_COMPILER "../bin/ccache_clang++")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -g -Wall -Werror -Weverything -Wno-c++98-compat-pedantic -Wno-disabled-macro-expansion -Wno-padded -Wno-weak-vtables -pedantic-errors -Wno-global-constructors -Wno-exit-time-destructors  -isystem .. -I ../src")
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=gold ")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")

find_package (Boost 1.62 COMPONENTS system program_options REQUIRED)
find_package (Threads)

file(GLOB_RECURSE proj_lib_sources src/*.cpp)
list(REMOVE_ITEM  proj_lib_sources 
     /code/src/main.cpp
)

set(PROJECT_NAME_LIB ${PROJECT_NAME}_lib)
add_library(${PROJECT_NAME}_lib OBJECT ${proj_lib_sources} 
    vendor/sha2-1.0.1/sha2.c
)
target_include_directories (${PROJECT_NAME_LIB} 
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

ADD_CUSTOM_COMMAND(
    COMMAND clang /code/vendor/sha2-1.0.1/sha2.c -c -o sha2.o
    OUTPUT sha2.o
    )

add_executable(${PROJECT_NAME} 
    $<TARGET_OBJECTS:${PROJECT_NAME_LIB}> src/main.cpp sha2.o)
target_link_libraries(${PROJECT_NAME}  
    -lpthread
    -lcurl
    -lboost_system
    -lboost_program_options
    /usr/local/lib/libcurlcpp.a
)
