cmake_minimum_required(VERSION 2.5)
project(set)




set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -g -D_GLIBCXX_DEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(main
        mm/main.cpp
        testing.cpp
        set.h
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc)

include_directories(${CMAKE_SOURCE_DIR})
target_link_libraries(main -lpthread)