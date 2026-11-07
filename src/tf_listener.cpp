#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include <chrono>

using namespace std::chrono_literals;

class TFListener : public rclcpp::Node
{
public:
    TFListener() : Node("tf_listener")
    {
        // 【1. 拿一个小本本 (Buffer)】
        buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        
        // 【2. 雇一个耳朵 (Listener)】，把听到的东西写进小本本里
        listener_ = std::make_shared<tf2_ros::TransformListener>(*buffer_);

        // 【3. 定个闹钟，每秒去翻一次小本本】
        timer_ = this->create_wall_timer(1s, std::bind(&TFListener::lookup_tf, this));
        
        RCLCPP_INFO(this->get_logger(), "TF 窃听器已启动，开始查岗...");
    }

private:
    void lookup_tf()
    {
        try
        {
            // 【核心动作：查账！】
            // 查谁？查 my_robot 相对于 world 的位置，查最新时刻的
            geometry_msgs::msg::TransformStamped t;
            t = buffer_->lookupTransform("world", "my_robot", tf2::TimePointZero);

            // 打印查到的结果（x 和 y 坐标）
            RCLCPP_INFO(this->get_logger(), "查到啦！my_robot 在: x=%.2f, y=%.2f", 
                        t.transform.translation.x, t.transform.translation.y);
        }
        catch (const tf2::TransformException & ex)
        {
            // 如果没查到（比如发布 TF 的节点没开），就会报错
            RCLCPP_WARN(this->get_logger(), "查不到坐标: %s", ex.what());
        }
    }

    std::shared_ptr<tf2_ros::Buffer> buffer_;
    std::shared_ptr<tf2_ros::TransformListener> listener_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TFListener>());
    rclcpp::shutdown();
    return 0;
}
