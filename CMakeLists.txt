cmake_minimum_required(VERSION 3.8)
project(demo_cpp_topic)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)
find_package(demo_interfaces REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

add_executable(turtle_circle src/turtle_circle.cpp)
ament_target_dependencies(turtle_circle rclcpp geometry_msgs)
install(TARGETS turtle_circle
  DESTINATION lib/${PROJECT_NAME}
)
add_executable(turtle_rose src/turtle_rose.cpp)
ament_target_dependencies(turtle_rose rclcpp geometry_msgs)
install(TARGETS turtle_rose
  DESTINATION lib/${PROJECT_NAME}
)

add_executable(turtle_follower src/turtle_follower.cpp)
ament_target_dependencies(turtle_follower rclcpp geometry_msgs turtlesim)
install(TARGETS turtle_follower
  DESTINATION lib/${PROJECT_NAME}
)
add_executable(sys_monitor src/sys_monitor.cpp)
ament_target_dependencies(sys_monitor rclcpp demo_interfaces)
install(TARGETS sys_monitor
  DESTINATION lib/${PROJECT_NAME}
)
add_executable(add_server src/add_server.cpp)
ament_target_dependencies(add_server rclcpp demo_interfaces)
install(TARGETS add_server
  DESTINATION lib/${PROJECT_NAME}
)
add_executable(add_client src/add_client.cpp)
ament_target_dependencies(add_client rclcpp demo_interfaces)
install(TARGETS add_client
  DESTINATION lib/${PROJECT_NAME}
)
add_executable(param_node src/param_node.cpp)
ament_target_dependencies(param_node rclcpp)
install(TARGETS param_node
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)
add_executable(static_tf src/static_tf.cpp)
ament_target_dependencies(static_tf rclcpp tf2_ros geometry_msgs)
install(TARGETS static_tf
  DESTINATION lib/${PROJECT_NAME}
)
add_executable(dynamic_tf src/dynamic_tf.cpp)
ament_target_dependencies(dynamic_tf rclcpp tf2_ros geometry_msgs)
install(TARGETS dynamic_tf
  DESTINATION lib/${PROJECT_NAME}
)
add_executable(tf_listener src/tf_listener.cpp)
ament_target_dependencies(tf_listener rclcpp tf2_ros geometry_msgs)
install(TARGETS tf_listener
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY launch urdf
  DESTINATION share/${PROJECT_NAME}
)
add_executable(turtle_keyboard src/turtle_keyboard.cpp)
ament_target_dependencies(turtle_keyboard rclcpp geometry_msgs)
install(TARGETS turtle_keyboard DESTINATION lib/${PROJECT_NAME})
add_executable(fake_odom src/fake_odom.cpp)
ament_target_dependencies(fake_odom rclcpp geometry_msgs tf2_ros tf2_geometry_msgs)
install(TARGETS fake_odom DESTINATION lib/${PROJECT_NAME})

ament_package()
