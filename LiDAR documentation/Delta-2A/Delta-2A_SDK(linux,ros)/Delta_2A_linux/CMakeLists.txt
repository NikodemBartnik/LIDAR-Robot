cmake_minimum_required(VERSION 2.8.3)
project(delta_2b_lidar)

set(DELTA_2B_LIDAR_SDK_PATH "./src/")

FILE(GLOB DELTA_2B_LIDAR_SDK_SRC 
  "${DELTA_2B_LIDAR_SDK_PATH}/src/*.cpp"
)

include_directories(
  ${DELTA_2B_LIDAR_SDK_PATH}/include
  ${DELTA_2B_LIDAR_SDK_PATH}/src
)

add_executable(delta_2b_lidar_node app/node.cpp ${DELTA_2B_LIDAR_SDK_SRC})
target_link_libraries(delta_2b_lidar_node -lrt -lpthread)


