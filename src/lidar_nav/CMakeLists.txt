cmake_minimum_required(VERSION 3.8)
project(lidar_nav)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)

add_executable(lidar_navigator src/lidar_nav.cpp)

ament_target_dependencies(lidar_navigator
  rclcpp
  sensor_msgs
  geometry_msgs
  nav_msgs
)

install(TARGETS lidar_navigator
  DESTINATION lib/${PROJECT_NAME})

ament_package()