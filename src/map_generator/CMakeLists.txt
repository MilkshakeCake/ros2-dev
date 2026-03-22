cmake_minimum_required(VERSION 3.8)
project(map_generator)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ament_index_cpp REQUIRED)

# 2. Define the executable
# This creates a binary named 'map_generator' from your .cpp file
add_executable(map_generator src/map_generator.cpp)

# Link ROS2 libraries to the executable
ament_target_dependencies(map_generator 
  rclcpp 
  ament_index_cpp
)

# 3. INSTALLATION (CRITICAL STEP)
# This moves the 'map_generator' binary from the build folder 
# to install/map_generator/lib/map_generator/
install(TARGETS map_generator
  DESTINATION lib/${PROJECT_NAME})

# 4. Export the package
ament_package()