cmake_minimum_required(VERSION 3.14)
project(ffttest)

set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
include_directories(${OpenCV_INCLUDE_DIRS})
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES
        Sound.h
        audioFFT.cpp
#        main.cpp
        )

add_executable(ffttest ${SOURCE_FILES})

target_link_libraries(ffttest ${OpenCV_LIBS})
target_link_libraries(ffttest fftw3)
target_link_libraries(ffttest openal)
