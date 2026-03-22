from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess, TimerAction
from launch_ros.actions import Node

# This node takes your URDF file and publishes it to the /robot_description topic
robot_state_publisher = Node(
    package='robot_state_publisher',
    executable='robot_state_publisher',
    parameters=[{'robot_description': open("/home/mlkshkck/dev_ws/src/my_lidar_bot/urdf/robot.urdf").read()}]
)

def generate_launch_description():
    
    # 1. Define the Map Generator Node
    map_gen_node = Node(
        package='map_generator',
        executable='map_generator',
        name='map_gen_node'
    )

    # 2. Define the Navigator Node
    nav_node = Node(
        package='lidar_nav',
        executable='lidar_nav',
        name='navigator',
        output='screen'
    )

    # 3. Create a Timer to delay the Navigator by 5 seconds
    delayed_nav_node = TimerAction(
        period=5.0,
        actions=[nav_node]
    )

    return LaunchDescription([
        map_gen_node,
        delayed_nav_node
    ])