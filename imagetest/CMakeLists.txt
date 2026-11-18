cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(imagetest)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(imagetest test_imagep.cpp)
target_link_libraries(imagetest "${TORCH_LIBRARIES}")



#cmake_minimum_required(VERSION 4.1.1)
#project(imagetest LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

#find_package(OpenCV REQUIRED)

#add_executable(imagetest test_imagep.cpp)

#target_include_directories(imagetest PRIVATE
#    ${OpenCV_INCLUDE_DIRS}
#)

#target_link_libraries(imagetest PRIVATE
#    ${OpenCV_LIBS}
#)
