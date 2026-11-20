cmake_minimum_required(VERSION 4.1.1)
project(cap2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# X11 library is required by CImg:
find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})
# include CImg header: 
include_directories(./CImg)
# Find Torch library
find_package(Torch REQUIRED)

add_executable(cap2 
    main.cpp
    imageprocessing_impl.cpp
    audioprocessing_impl.cpp
)

target_sources(cap2
    PUBLIC
        FILE_SET CXX_MODULES 
        FILES
            imageprocessing.cppm
            audioprocessing.cppm
)

target_link_libraries(cap2 
    ${TORCH_LIBRARIES}
)


