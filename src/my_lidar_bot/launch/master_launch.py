import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import ExecuteProcess, TimerAction
from launch_ros.actions import Node

def generate_launch_description():
    # 1. Setup Package Paths
    # Ensure these names match your folders in ~/dev_ws/src/
    pkg_lidar_bot = get_package_share_directory('my_lidar_bot')
    pkg_map_gen = get_package_share_directory('map_generator')
    pkg_nav = get_package_share_directory('lidar_nav') # Renamed from lidar_nav_logic

    # Path to URDF
    urdf_file = os.path.join(pkg_lidar_bot, 'urdf', 'robot.urdf')
    
    # Path to World (created by map_generator)
    world_path = os.path.join(pkg_map_gen, 'random_map.world')

    # 2. Define the Mission Parameters
    # This is shared by the Map Generator (for the pole) and Navigator (for the math)
    goal_params = [{'goal_x': 5.0, 'goal_y': 5.0}]

    # 3. Map Generation Node
    map_gen_node = Node(
        package='map_generator',
        executable='map_generator',
        name='map_generator',
        parameters=goal_params,
        output='screen'
    )

    # 4. Robot State Publisher (URDF broadcaster)
    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': open(urdf_file).read()}]
    )

    # 5. Gazebo (Starts the simulator)
    gazebo = ExecuteProcess(
        cmd=['gazebo', '--verbose', '-s', 'libgazebo_ros_factory.so', world_path],
        output='screen'
    )

    # 6. Spawn Entity (Puts the robot in Gazebo)
    spawn_entity = Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=['-topic', 'robot_description', '-entity', 'my_lidar_bot', '-z', '0.1'],
        output='screen'
    )

    # 7. Lidar Navigator (The Brain)
    # Delayed 5 seconds to ensure Gazebo and the World are fully loaded
    nav_node = Node(
    package='lidar_nav',
    executable='lidar_navigator', # This must match the add_executable name in CMake
    name='navigator',
    parameters=goal_params,
    output='screen'
)

    delayed_nav = TimerAction(
        period=5.0,
        actions=[nav_node]
    )

    return LaunchDescription([
        map_gen_node,
        robot_state_publisher,
        gazebo,
        spawn_entity,
        delayed_nav
    ])