cmake_minimum_required(VERSION 3.10)
project(adrv9025-iiostream)

# 设置 C++ 标准为 C++11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(INCLUDE ${PROJECT_SOURCE_DIR}/include)

include_directories(${PROJECT_SOURCE_DIR}
${INCLUDE}
)

# 添加源文件
add_library(iio-helper
src/iiostream-set-attribution.c
src/lsb2msb.c
src/lfm.c)  # 创建一个库目标


add_executable(
    adrv9025-iiostream adrv9025-iiostream.c ${GETOPT_C_FILE} ${LIBIIO_RC}
)

target_link_libraries(adrv9025-iiostream -lm -liio iio-helper)
