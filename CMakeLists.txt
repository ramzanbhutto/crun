cmake_minimum_required(VERSION 3.16)
project(crun CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

add_executable(crun
    src/main.cpp
    src/namespaces.cpp
    src/child.cpp
    src/mounts.cpp
    src/cgroup.cpp
)
