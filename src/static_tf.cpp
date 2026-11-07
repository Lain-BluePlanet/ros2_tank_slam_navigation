#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/static_transform_broadcaster.h"
#include "geometry_msgs/msg/transform_stamped.hpp"

class StaticTFBroadcaster : public rclcpp::Node
{
public:
    StaticTFBroadcaster() : Node("static_tf_broadcaster")
    {
        // 1. 创建静态 TF 广播器
        tf_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

        // 2. 准备要发布的数据
        geometry_msgs::msg::TransformStamped t;
        
        // 时间戳
        t.header.stamp = this->get_clock()->now();
        
        // 【核心逻辑：父子关系】
        // 我们设定 world (世界) 是爸爸，my_robot (机器人) 是儿子
        t.header.frame_id = "world"; 
        t.child_frame_id = "my_robot";

        // 【核心逻辑：位置与旋转】
        // 假设我的机器人在世界坐标系的 (1.0, 2.0, 3.0) 位置
        t.transform.translation.x = 1.0;
        t.transform.translation.y = 2.0;
        t.transform.translation.z = 3.0;

        // 默认不旋转（四元数，不用管，填这四个数就行）
        t.transform.rotation.x = 0.0;
        t.transform.rotation.y = 0.0;
        t.transform.rotation.z = 0.0;
        t.transform.rotation.w = 1.0;

        // 3. 发送！
        tf_broadcaster_->sendTransform(t);
        RCLCPP_INFO(this->get_logger(), "静态TF已发布：world -> my_robot");
    }

private:
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<StaticTFBroadcaster>());
    rclcpp::shutdown();
    return 0;
}
