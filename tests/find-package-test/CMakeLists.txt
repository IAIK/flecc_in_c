cmake_minimum_required(VERSION 2.8.12)

project(flecc_in_c-test)

find_package(flecc_in_c REQUIRED)

add_executable(library-test "main.c")
target_link_libraries(library-test flecc_in_c)
