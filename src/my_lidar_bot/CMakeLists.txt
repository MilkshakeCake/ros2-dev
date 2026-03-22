cmake_minimum_required(VERSION 3.8)
project(my_lidar_bot)

# 1. We only need ament_cmake to handle the "Copying" (Installation)
find_package(ament_cmake REQUIRED)

# 2. INSTALLATION: This simply copies your folders to the 'install' space
# No 'install(TARGETS...)' is needed because there is no compiled code.
install(DIRECTORY urdf 
  DESTINATION share/${PROJECT_NAME})

install(DIRECTORY launch 
  DESTINATION share/${PROJECT_NAME})

# 3. Export the package
ament_package()