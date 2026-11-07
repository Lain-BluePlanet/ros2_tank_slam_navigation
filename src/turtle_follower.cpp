#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include <cmath>

class TurtleFollower : public rclcpp::Node
{
public:
    TurtleFollower() : Node("turtle_follower_node")
    {
        // 绿乌龟发速度
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
        
        // 绿乌龟看自己的位置
        sub_turtle2_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle2/pose", 10, std::bind(&TurtleFollower::pose2_callback, this, std::placeholders::_1));

        // 绿乌龟偷看红乌龟的位置 (这就是追踪的核心)
        sub_turtle1_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose", 10, std::bind(&TurtleFollower::pose1_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "追踪程序启动！用键盘控制红乌龟跑起来吧！");
    }

private:
    // 记录红乌龟的位置
    void pose1_callback(const turtlesim::msg::Pose::SharedPtr msg) { target_x_ = msg->x; target_y_ = msg->y; }
    
    // 绿乌龟的脑子（核心算法）
    void pose2_callback(const turtlesim::msg::Pose::SharedPtr msg)
    {
        double diff_x = target_x_ - msg->x;
        double diff_y = target_y_ - msg->y;
        double distance = std::sqrt(diff_x * diff_x + diff_y * diff_y);

        auto cmd = geometry_msgs::msg::Twist();

        // 如果离红乌龟小于 1.5 的距离，就原地打转，不追了
        if (distance < 1.5) {
            cmd.linear.x = 0.0;
            cmd.angular.z = 1.0; 
        } else {
            // 否则，算角度冲过去
            double target_theta = std::atan2(diff_y, diff_x);
            double diff_theta = target_theta - msg->theta;
            
            // 限制角度
            while (diff_theta > M_PI) diff_theta -= 2.0 * M_PI;
            while (diff_theta < -M_PI) diff_theta += 2.0 * M_PI;

            cmd.linear.x = std::min(2.0 * distance, 3.0); // 距离越远，跑得越快（最快速度限制在3.0）
            cmd.angular.z = 3.0 * diff_theta;
        }
        publisher_->publish(cmd);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_turtle1_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_turtle2_;
    double target_x_ = 5.5;
    double target_y_ = 5.5;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleFollower>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
