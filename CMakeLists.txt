cmake_minimum_required(VERSION 3.10)

project(lab1-state-space-searching)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

cmake_policy(SET CMP0167 NEW)
find_package(Boost REQUIRED COMPONENTS system graph)

add_executable(main 
    main.cpp
)
target_compile_options(main PRIVATE 
    -O3
    -mavx
    -fno-exceptions 
    -fno-rtti 
    -funroll-loops )

target_link_libraries(main PRIVATE ${Boost_LIBRARIES})
