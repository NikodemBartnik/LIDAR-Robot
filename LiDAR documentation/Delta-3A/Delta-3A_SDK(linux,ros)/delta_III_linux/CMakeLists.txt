cmake_minimum_required(VERSION 2.8.3)
project(delta_lidar)

set(delta_lidar_SDK_PATH "./src/")

FILE(GLOB delta_lidar_SDK_SRC 
  "${delta_lidar_SDK_PATH}/src/*.cpp"
)

include_directories(
  ${delta_lidar_SDK_PATH}/include
  ${delta_lidar_SDK_PATH}/src
)

add_executable(delta_lidar_node app/node.cpp ${delta_lidar_SDK_SRC})
target_link_libraries(delta_lidar_node -lrt -lpthread)


