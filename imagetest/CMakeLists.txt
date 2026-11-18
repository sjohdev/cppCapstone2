cmake_minimum_required(VERSION 4.1.1)
project(imagetest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)

# System fortran runtime
set(GFORTRAN_LIBRARY "/usr/lib64/libgfortran.so.5")

if(NOT EXISTS "${GFORTRAN_LIBRARY}")
    message(FATAL ERROR "System libgfortran not found!")
endif()

add_executable(imagetest test_imagep.cpp)

target_include_directories(imagetest PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${TORCH_INCLUDE_DIRS}
)

target_link_options(imagetest PRIVATE
    "-Wl,--no-as-needed"
)

target_link_libraries(imagetest PRIVATE
    ${OpenCV_LIBS}
    ${TORCH_LIBRARIES}
    ${GFORTRAN_LIBRARY}
)

# LibTorch compiler flags
separate_arguments(TORCH_CXX_FLAGS_LIST
    NATIVE_COMMAND "${TORCH_CXX_FLAGS}"
)

target_compile_options(imagetest PRIVATE
    ${TORCH_CXX_FLAGS_LIST}

)

# Runtime search path
set_target_properties(imagetest PROPERTIES
    BUILD_RPATH "usr/lib64"
    INSTALL_RPATH "/usr/lib64"
)

