cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(imagetest)
# X11 library is required by CImg:
find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})
# include CImg header: 
include_directories(../CImg)
# Find Torch library
find_package(Torch REQUIRED)

add_executable(imagetest test_imagep.cpp)

target_link_libraries(imagetest 
    ${TORCH_LIBRARIES}
)


