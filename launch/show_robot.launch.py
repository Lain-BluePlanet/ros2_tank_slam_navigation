import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import xacro

def generate_launch_description():
    # 用 xacro 模块解析 xacro 文件
    xacro_file = '/home/blueplanet/chapt2_ws/src/demo_cpp_topic/urdf/my_robot.xacro'
    doc = xacro.parse(open(xacro_file))
    xacro.process_doc(doc)
    robot_desc = doc.toxml()

    return LaunchDescription([
        Node(
            package='joint_state_publisher_gui',
            executable='joint_state_publisher_gui',
            name='joint_state_publisher_gui'
        ),
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_desc}]
        ),
        Node(
            package='rviz2',
            executable='rviz2'
        ),
        # 新增：键盘控制节点
        Node(
            package='demo_cpp_topic',
            executable='turtle_keyboard',
            name='keyboard_control_node',
            prefix='xterm -e' # 重点：强制弹出一个新窗口来捕捉键盘，否则按键没反应
        ),
        Node(
            package='demo_cpp_topic',
            executable='fake_odom',
            name='fake_odom'
        )
    ])
