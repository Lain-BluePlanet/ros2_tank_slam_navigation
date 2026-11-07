#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <chrono>
#include <cmath> // 引入数学库，用来算三角函数

using namespace std::chrono_literals;

class TurtleRose : public rclcpp::Node
{
public:
    TurtleRose() : Node("turtle_rose_node")
    {
        // 注意！这里把目标改成了 turtle2（第二只乌龟的频道）
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
        
        // 用毫秒做定时器，每 20 毫秒算一次速度（这让运动非常平滑）
        timer_ = this->create_wall_timer(20ms, std::bind(&TurtleRose::timer_callback, this));
        
        // 初始化时间变量
        time_ = 0.0;
        
        RCLCPP_INFO(this->get_logger(), "玫瑰线绘制启动！请看 turtle2！");
    }

private:
    void timer_callback()
    {
        auto msg = geometry_msgs::msg::Twist();

        // 1. 时间往前走一点点（每次增加 0.02 秒）
        time_ += 0.02;

        // 2. 核心魔法：用三角函数算出不停变化的线速度和角速度
        // 这里的数字你可以自己改，比如把 3.0 改成 5.0 花瓣就会变多！
        msg.linear.x = 2.5 * std::sin(3.0 * time_); 
        msg.angular.z = 1.5 + 2.0 * std::cos(3.0 * time_);

        // 把算好的速度发出去
        publisher_->publish(msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    double time_; // 记录时间流逝的变量
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleRose>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
