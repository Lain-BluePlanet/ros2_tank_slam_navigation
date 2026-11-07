#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include <cmath>

using namespace std::chrono_literals;

class FakeOdom : public rclcpp::Node {
public:
    FakeOdom() : Node("fake_odom") {
        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        
        // 订阅 /turtle1/cmd_vel
        sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/turtle1/cmd_vel", 10, std::bind(&FakeOdom::cmd_callback, this, std::placeholders::_1));
            
        // 定时器：30Hz 发送 TF
        timer_ = this->create_wall_timer(33ms, std::bind(&FakeOdom::broadcast_tf, this));
        x_ = 0.0; y_ = 0.0; theta_ = 0.0;
    }

private:
    void cmd_callback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        current_v_ = msg->linear.x;
        current_w_ = msg->angular.z;
    }

    void broadcast_tf() {
        double dt = 0.033; // 时间间隔
        x_ += current_v_ * cos(theta_) * dt;
        y_ += current_v_ * sin(theta_) * dt;
        theta_ += current_w_ * dt;

        geometry_msgs::msg::TransformStamped t;
        t.header.stamp = this->get_clock()->now();
        t.header.frame_id = "odom"; 
        t.child_frame_id = "base_link"; 

        t.transform.translation.x = x_;
        t.transform.translation.y = y_;
        t.transform.translation.z = 0.0;
        
        tf2::Quaternion q;
        q.setRPY(0, 0, theta_);
        t.transform.rotation = tf2::toMsg(q);

        tf_broadcaster_->sendTransform(t);
    }

    // 这里补充了完整的变量声明，不再用“略”了！
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    double x_, y_, theta_;
    double current_v_ = 0.0, current_w_ = 0.0;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<FakeOdom>());
    rclcpp::shutdown();
    return 0;
}