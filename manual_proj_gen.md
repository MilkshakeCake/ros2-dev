# ROS2 Project Generation Manual# ROS2 Project Generation Manual (C++ & Python Launch)

This manual provides a standardized workflow for initializing, coding, and launching a ROS2 project using C++ for logic and Python for orchestration.

---

## 1. Workspace & Package Initialization

- **Navigate to Workspace:**
  ```bash
  mkdir -p ~/ros2_ws/src
  cd ~/ros2_ws/src
  ```
- **Create Package:**
  ```bash
  ros2 pkg create --build-type ament_cmake <pkg_name> --dependencies rclcpp std_msgs
  ```

---

## 2. System Metadata (package.xml)

This file tells the ROS2 build system which libraries to install. **Critical for `rosdep`.**

```xml
<package format="3">
  <name>your_package_name</name>
  <version>0.0.0</version>
  <description>Robot system software package</description>
  <maintainer email="ollehme@gmail.com">mlkshkck</maintainer>
  <license>Apache License 2.0</license>

  <buildtool_depend>ament_cmake</buildtool_depend>

  <depend>rclcpp</depend>
  <depend>std_msgs</depend>

  <export>
    <build_type>ament_cmake</build_type>
  </export>
</package>
```

---

## 3. Baseline Node (src/my_node.cpp)

Use this Object-Oriented (OOP) template. It is more scalable than a single-function script.

**File: src/my_node.cpp**

```cpp
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class MyNode : public rclcpp::Node {
public:
  MyNode() : Node("my_node_name") {
    // Initialize Publisher (Topic: "chatter", Queue size: 10)
    publisher_ = this->create_publisher<std_msgs::msg::String>("chatter", 10);

    // Initialize Timer (1Hz / every 1000ms)
    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(1000),
      std::bind(&MyNode::timer_callback, this));

    RCLCPP_INFO(this->get_logger(), "C++ Node started successfully.");
  }

private:
  void timer_callback() {
    auto message = std_msgs::msg::String();
    message.data = "Hello from C++ Node!";
    publisher_->publish(message);
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MyNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
```

---

## 4. Build Logic(CMakeLists.txt) - C++ Only

```cmake
cmake_minimum_required(VERSION 3.8)
project(your_package_name)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# 2. Add Executable (Repeat for each node)
add_executable(my_node src/my_node.cpp)

# 3. Link dependencies to the executable
ament_target_dependencies(my_node rclcpp std_msgs)

# 4. Install executable (So 'ros2 run' can find it)
install(TARGETS
  my_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
```

---

## 5. Python Launch File (launch/my_launch.py)

Even in C++ projects, Python launch files are the standard for orchestration. Create a launch/ folder and add this file.

```python
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='your_package_name',
            executable='my_node',
            name='controller_node',
            output='screen',
            parameters=[{'use_sim_time': True}]
        )
    ])
```

---

## 6. The Build & Source Workflow

Follow this specific sequence from the workspace root (`~/dev_ws`) to ensure the environment recognizes the new project.

1.  **Resolve Dependencies:**
    ```bash
    rosdep install -i --from-path src --rosdistro humble -y
    ```
2.  **Build with Symlinks:**
    _(Recommended for Python/Config updates without rebuilding)_
    ```bash
    colcon build --symlink-install
    ```
3.  **Source the Overlay:**
    ```bash
    source install/setup.bash
    ```
4.  **Run:**
    ```bash
    ros2 launch your_package_name my_launch.py
    ```

---

## 7. VS Code Environment Setup

To enable IntelliSense and remove "red squiggles" in WSL2:

- **C++:** Update `.vscode/c_cpp_properties.json` to include:
  `"/opt/ros/humble/include/**"`
- **Python:** Set your Python Interpreter to:
  `/usr/bin/python3`

---

### Quick Reference: `package.xml` vs `CMakeLists.txt`

| File                 | Targeted By        | Primary Job                                |
| :------------------- | :----------------- | :----------------------------------------- |
| **`package.xml`**    | `rosdep` / `ament` | Defines library dependencies (Debian/Apt). |
| **`CMakeLists.txt`** | `cmake` / `make`   | Compiles source code and links libraries.  |

---

## Troubleshooting Common Generation Issues

> **Note:** If `colcon build` fails to find your package, verify that the package directory is located directly inside the `src` folder.
