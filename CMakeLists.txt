cmake_minimum_required(VERSION 3.20)
project(VisprogCXXBackend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(ZeroMQ REQUIRED)
find_package(cppzmq REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(server main.cpp)


target_link_libraries(server PRIVATE 
    cppzmq
    nlohmann_json::nlohmann_json
)


target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/include)