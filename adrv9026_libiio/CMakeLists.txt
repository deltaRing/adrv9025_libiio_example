cmake_minimum_required(VERSION 3.10)
project(adrv9025-iiostream)

# 设置 C++ 标准为 C++11
set(CMAKE_C++_STANDARD 11)
set(CMAKE_C++_STANDARD_REQUIRED ON)
# 设置为 Release 编译模式
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(INCLUDE ${PROJECT_SOURCE_DIR}/include)

include_directories(${PROJECT_SOURCE_DIR}
${INCLUDE}
)

# 加入Eigen Include 以及 FFTW3
find_package(Eigen3 REQUIRED)
find_package(FFTW3)
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})

if(FFTW3_FOUND)
    include_directories(${FFTW3_INCLUDE_DIRS})
    link_directories(${FFTW3_LIBRARY_DIRS})
else(FFTW3_FOUND)
    message(FATAL_ERROR "FFTW3 library not found")
endif(FFTW3_FOUND)

# 添加源文件
add_library(iio-helper
src/Utilize/iiostream-set-attribution.cpp
src/Utilize/data_convert.cpp
src/Utilize/test_signal.cpp
src/Algorithms/LFM.cpp
src/Algorithms/MVDR.cpp
src/Algorithms/BeamPosing.cpp
src/Algorithms/Beamforming.cpp
src/Algorithms/PulseDetection.cpp
src/FMCW_MIMO/FMCW_LFM.cpp)  # 创建一个库目标


add_executable(
    adrv9025-iiostream adrv9025-iiostream.cpp ${GETOPT_C_FILE} ${LIBIIO_RC}
)

target_link_libraries(adrv9025-iiostream -lm -liio iio-helper fftw3 -lpthread)
