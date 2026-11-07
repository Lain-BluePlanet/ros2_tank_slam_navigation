#include "rclcpp/rclcpp.hpp"

class ParamNode : public rclcpp::Node
{
public:
    ParamNode() : Node("param_node")
    {
        // 【核心1：声明参数】
        // 声明一个叫 "my_speed" 的参数，默认值是 2.5
        this->declare_parameter("my_speed", 2.5);

        // 【核心2：获取参数】
        // 从系统里把 "my_speed" 的值拿出来
        double current_speed = this->get_parameter("my_speed").as_double();

        RCLCPP_INFO(this->get_logger(), "节点启动成功！当前速度参数 = %.2f", current_speed);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ParamNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
