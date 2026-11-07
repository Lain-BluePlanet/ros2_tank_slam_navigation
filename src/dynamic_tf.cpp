#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include <chrono>
#include <cmath>

using namespace std::chrono_literals;

class DynamicTFBroadcaster : public rclcpp::Node
{
public:
    DynamicTFBroadcaster() : Node("dynamic_tf_broadcaster")
    {
        // 【区别1：去掉 Static，买一个动态大喇叭】
        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);

        // 【区别2：加个定时器！每 100 毫秒（0.1秒）疯狂执行一次 send_tf】
        timer_ = this->create_wall_timer(100ms, std::bind(&DynamicTFBroadcaster::send_tf, this));
        
        time_ = 0.0;
        RCLCPP_INFO(this->get_logger(), "动态TF已启动！机器人开始绕圈！");
    }

private:
    void send_tf()
    {
        geometry_msgs::msg::TransformStamped t;
        
        // 【区别3：时间戳必须每次实时更新】
        t.header.stamp = this->get_clock()->now();
        
        t.header.frame_id = "world"; 
        t.child_frame_id = "my_robot";

        // 【魔法：让坐标动起来！】
        // 时间每次加 0.1
        time_ += 0.1;
        
        // 用三角函数算位置，这就是数学的魅力！
        // 机器人会在 X 和 Y 轴上画一个半径 2 米的圆
        t.transform.translation.x = 2.0 * std::cos(time_);
        t.transform.translation.y = 2.0 * std::sin(time_);
        t.transform.translation.z = 0.0; // 高度不变

        // 默认旋转（这里为了简单先不转，只平移）
        t.transform.rotation.x = 0.0;
        t.transform.rotation.y = 0.0;
        t.transform.rotation.z = 0.0;
        t.transform.rotation.w = 1.0;

        // 发出去！
        tf_broadcaster_->sendTransform(t);
    }

    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    rclcpp::TimerBase::SharedPtr timer_;
    double time_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DynamicTFBroadcaster>());
    rclcpp::shutdown();
    return 0;
}
