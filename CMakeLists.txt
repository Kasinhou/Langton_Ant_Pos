cmake_minimum_required(VERSION 3.18)
project(sem_Ants C)

set(CMAKE_C_STANDARD 11)

add_executable(sem_Ants main.c)
target_link_libraries(sem_Ants pthread)