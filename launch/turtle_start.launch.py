from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess, TimerAction

def generate_launch_description():
    ld = LaunchDescription()

    # 【场景 1：深色背景】
    ld.add_action(Node(
        package='turtlesim',
        executable='turtlesim_node',
        name='turtlesim',
        parameters=[{'background_r': 20, 'background_g': 20, 'background_b': 30}]
    ))

    # 【场景 2：把 3 只乌龟召唤在正中心附近，防止撞墙】
    # 注意看坐标，我们让它们在 (5.5, 5.5) 附近稍微散开
    spawn_actions = []
    
    # 画圆的乌龟，放在偏左一点
    spawn_actions.append(ExecuteProcess(
        cmd=['ros2', 'service', 'call', '/spawn', 'turtlesim/srv/Spawn', 
             "{x: 3.0, y: 5.5, theta: 0.0, name: 'turtle2'}"], output='screen'))
             
    # 画玫瑰的乌龟1，放在正中间
    spawn_actions.append(ExecuteProcess(
        cmd=['ros2', 'service', 'call', '/spawn', 'turtlesim/srv/Spawn', 
             "{x: 5.5, y: 5.5, theta: 0.0, name: 'turtle3'}"], output='screen'))
             
    # 画玫瑰的乌龟2，放在偏右一点
    spawn_actions.append(ExecuteProcess(
        cmd=['ros2', 'service', 'call', '/spawn', 'turtlesim/srv/Spawn', 
             "{x: 8.0, y: 5.5, theta: 0.0, name: 'turtle4'}"], output='screen'))

    ld.add_action(TimerAction(period=2.0, actions=spawn_actions))

    # 【场景 3：启动画图程序】
    draw_actions = []
    
    # 让 turtle2 画圆
    draw_actions.append(Node(
        package='demo_cpp_topic', executable='turtle_circle',
        remappings=[('/turtle1/cmd_vel', '/turtle2/cmd_vel')]
    ))
    
    # 让 turtle3 画玫瑰
    draw_actions.append(Node(
        package='demo_cpp_topic', executable='turtle_rose',
        remappings=[('/turtle2/cmd_vel', '/turtle3/cmd_vel')] # 注意你原来的 rose 代码里可能写死了 turtle2，这里重映射过去
    ))

    # 让 turtle4 也画玫瑰
    draw_actions.append(Node(
        package='demo_cpp_topic', executable='turtle_rose',
        remappings=[('/turtle2/cmd_vel', '/turtle4/cmd_vel')]
    ))

    ld.add_action(TimerAction(period=4.0, actions=draw_actions))

    return ld
