# 🛠 ROS 2 C++ Project Management & Troubleshooting Manual

This manual outlines the precise configuration steps required to maintain a "flawless" build environment for C++ nodes in ROS 2 Foxy.

---

## 1. Dependency Synchronization

A flawless build requires the **Manifest** and the **Build System** to be in perfect sync.

### Step A: The `package.xml` (The Contract)

This file tells the ROS 2 ecosystem what your package needs to exist.

- **Rule:** Use the `<depend>` tag for most ROS libraries to avoid redundant build/exec declarations.
- **Redundancy Check:** Ensure no library is listed twice.

### Step B: The `CMakeLists.txt` (The Instruction)

This file tells the C++ compiler exactly how to turn your code into a binary.

---

## 2. Hardened `CMakeLists.txt` Template

Use this structure to avoid linking errors. Replace project and node names with your specific targets.

```cmake
cmake_minimum_required(VERSION 3.5)
project(my_project)

# 1. FIND DEPENDENCIES
# Use REQUIRED to catch missing libraries at the start of the build
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

# 2. DEFINE EXECUTABLES
# Maps your source code (.cpp) to a binary name
add_executable(my_node src/my_node.cpp)

# 3. LINK DEPENDENCIES (The "Crucial" Step)
# This handles include paths and library linking automatically
ament_target_dependencies(my_node
  rclcpp
  geometry_msgs
  sensor_msgs
)

# 4. INSTALLATION
# Ensures 'ros2 run' can find your binaries
install(TARGETS
  my_node
  DESTINATION lib/${PROJECT_NAME}
)

# 5. EXPORT FOR OTHERS
# Must be the absolute last line of the file
ament_package()
```

---

## 3. The "Flawless Build" Checklist

Execute these steps in order whenever you make structural changes (adding files or libraries).

1. **Verify Manifest:** Add library to `package.xml`.
2. **Verify CMake:** Add library to `find_package` AND `ament_target_dependencies`.
3. **Resolve System Pkgs:**

```bash
cd ~/dev_ws
rosdep install --from-path src
```

4. **Purge and Rebuild:** (Recommended for structural changes)

```bash
rm -rf build/ install/ log/
colcon build --symlink-install --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

5. **Sync Environment:**

```bash
source install/setup.bash
```

---

## 4. VS Code IntelliSense Configuration

To eliminate red "include errors" while the terminal build succeeds:

1. Open **`.vscode/c_cpp_properties.json`**.
2. Ensure the `compilerPath` points to the Linux binary (not Windows).
3. Point `compileCommands` to the specific package folder created by `colcon`.

```json
{
  "configurations": [
    {
      "name": "Linux",
      "includePath": ["${workspaceFolder}/**", "/opt/ros/foxy/include/**"],
      "compilerPath": "/usr/bin/gcc",
      "cStandard": "gnu17",
      "cppStandard": "gnu++14",
      "intelliSenseMode": "linux-gcc-x64",
      "compileCommands": "${workspaceFolder}/build/${packageName}/compile_commands.json"
    }
  ],
  "version": 4
}
```

---

## 5. Common Error Resolutions

| Error Message                 | Likely Cause                      | Solution                               |
| :---------------------------- | :-------------------------------- | :------------------------------------- |
| `No such file or directory`   | Missing `find_package` or `link`. | Add library to `CMakeLists.txt`.       |
| `Package 'x' not found`       | Sourcing error.                   | Run `source install/setup.bash`.       |
| `Undefined reference`         | Linker failure.                   | Check `ament_target_dependencies`.     |
| `Failed to parse package.xml` | Duplicates.                       | Check for redundant `<depend>` tags.   |
| `IntelliSense squiggles`      | Wrong `compileCommands` path.     | Update JSON to match the `build/` pkg. |
