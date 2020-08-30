cmake_minimum_required(VERSION 3.16)
project(ICMMeteoLogger)

set(CMAKE_CXX_STANDARD 11)

find_package(CURL REQUIRED)
find_package(dkulpaclibs REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(ICMMeteoLogger main.cpp)
target_link_libraries(ICMMeteoLogger ${dkulpaclibs_misc_LIB} ${CURL_LIBRARIES} ${OpenCV_LIBS})