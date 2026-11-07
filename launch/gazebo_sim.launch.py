import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import xacro

def generate_launch_description():
    # 1. 解析 Xacro
    xacro_file = '/home/blueplanet/chapt2_ws/src/demo_cpp_topic/urdf/my_robot.xacro'
    doc = xacro.parse(open(xacro_file))
    xacro.process_doc(doc)
    robot_desc = doc.toxml()

    # 2. 引入 Gazebo 官方启动文件
    gazebo_ros_pkg = get_package_share_directory('gazebo_ros')
    gazebo_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(gazebo_ros_pkg, 'launch', 'gazebo.launch.py'))
    )

    return LaunchDescription([
        gazebo_launch,
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_desc}]
        ),
        # 直接召唤，强行等待 10 秒
        Node(
            package='gazebo_ros',
            executable='spawn_entity.py',
            arguments=['-topic', 'robot_description', '-entity', 'my_tank', '-timeout', '10.0','-z','2.0'],
            output='screen'
        ),

        # 👇 新增：启动你的键盘控制节点 👇
        Node(
            package='demo_cpp_topic',
            executable='turtle_keyboard',
            name='keyboard_control_node',
            prefix='xterm -e', # 弹出小黑框接收按键
            # 把键盘节点发出的话题，映射到 Gazebo 电机听的话题上
            remappings=[('/turtle1/cmd_vel', '/cmd_vel')] 
        )
    ])