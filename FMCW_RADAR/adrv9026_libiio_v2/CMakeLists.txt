cmake_minimum_required(VERSION 3.10)
project(adrv9025-iiostream)

# 设置 C++ 标准为 C++11
set(CMAKE_C++_STANDARD 14)
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
# src/DMA/dma_function.cpp
# src/DMA/libaxidma.cpp # Dont Use These
src/Utilize/iio_adrv9025.cpp
src/Utilize/iio_adrv9009.cpp
src/Utilize/data_convert.cpp
src/Utilize/common_define.cpp
src/Utilize/fast_load_data.cpp
src/Utilize/read_configuration.cpp
src/Algorithms/LFM.cpp
src/Algorithms/MVDR.cpp
src/Algorithms/BeamPosing.cpp
src/Algorithms/Beamforming.cpp
src/Algorithms/DownSampling.cpp
src/Algorithms/PulseDetection.cpp
src/Algorithms/RadarFunctions.cpp
src/Algorithms/BackgroundCancellation.cpp
src/LOL/LOL.cpp
src/FMCW_MIMO/FMCW_LFM.cpp
src/FMCW_MIMO/FMCW_MIMO_Algorithm.cpp
src/Settings/UsingDifferentModule.cpp
src/Settings/TestFFTTime.cpp
src/PulseRadar_LFM/PulseRadar_LFM.cpp
src/DMA_PROXY/dma_functions.cpp
src/DMA_PROXY/process_LFM.cpp
src/DMA_PROXY/dma_utilize.cpp
src/DMA_PROXY/AXI_LITE_4.cpp # Use These // Officials
src/Networks/UDP_Networks.cpp
src/Networks/TCP_Networks.cpp
)  # 创建一个库目标


add_executable(
    adrv9025-iiostream adrv9025-iiostream.cpp ${GETOPT_C_FILE} ${LIBIIO_RC}
)

target_link_libraries(adrv9025-iiostream -lm -lfftw3f -liio iio-helper fftw3 -lpthread)
