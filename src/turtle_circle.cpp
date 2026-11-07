#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <chrono>

using namespace std::chrono_literals;

class TurtleCircleNode : public rclcpp::Node
{
public:
    TurtleCircleNode() : Node("turtle_circle_node")
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        timer_ = this->create_wall_timer(100ms, std::bind(&TurtleCircleNode::timer_callback, this));
        RCLCPP_INFO(this->get_logger(), "转圈节点已启动！看乌龟！");
    }

private:
    void timer_callback()
    {
        auto msg = geometry_msgs::msg::Twist();
        msg.linear.x = 2.0;
        msg.angular.z = 1.0;
        publisher_->publish(msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleCircleNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
