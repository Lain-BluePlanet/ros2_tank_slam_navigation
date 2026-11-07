#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <termios.h>
#include <unistd.h>

// 这是一个用来读取键盘按键的神奇函数
int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

class KeyboardControl : public rclcpp::Node
{
public:
    KeyboardControl() : Node("keyboard_control_node")
    {
        // 创建发布者，发到 /cmd_vel 话题（真实机器人也认这个）
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        RCLCPP_INFO(this->get_logger(), "键盘控制节点已启动！按 W/A/S/D 移动，按 Q 退出。");

        // 开启一个定时器，疯狂监听键盘
        timer_ = this->create_wall_timer(std::chrono::milliseconds(50), std::bind(&KeyboardControl::check_key, this));
    }

private:
    void check_key()
    {
        int key = getch();
        auto msg = geometry_msgs::msg::Twist();

        msg.linear.x=0.0;
        msg.angular.z=0.0;

        if (key == 'w' || key == 'W') {
            msg.linear.x = 0.5; // 前进
            RCLCPP_INFO(this->get_logger(), "前进！");
        } else if (key == 's' || key == 'S') {
            msg.linear.x = -0.5; // 后退
            RCLCPP_INFO(this->get_logger(), "后退！");
        } else if (key == 'a' || key == 'A') {
            msg.linear.x=0.2;
            msg.angular.z = 1.0; // 左转
            RCLCPP_INFO(this->get_logger(), "左转！");
        } else if (key == 'd' || key == 'D') {
            msg.linear.x=0.2;
            msg.angular.z = -1.0; // 右转
            RCLCPP_INFO(this->get_logger(), "右转！");
        } else if (key == 'q' || key == 'Q') {
            RCLCPP_INFO(this->get_logger(), "退出节点。");
            rclcpp::shutdown();
        } else {
            publisher_->publish(msg);
            return; // 按了别的键，不发数据
        }
        
        publisher_->publish(msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<KeyboardControl>());
    rclcpp::shutdown();
    return 0;
}