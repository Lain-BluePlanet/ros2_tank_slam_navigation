#include "rclcpp/rclcpp.hpp"
#include "demo_interfaces/srv/add_two_ints.hpp"

class AddClient : public rclcpp::Node
{
public:
    AddClient() : Node("add_client_node")
    {
        // 【不同点1：买一部能打出去的电话】
        // 注意这里叫 create_client（创建客户端）
        client_ = this->create_client<demo_interfaces::srv::AddTwoInts>("add_two_ints");
        
        RCLCPP_INFO(this->get_logger(), "客户端已启动，准备发送请求...");
    }

    // 写一个专门发请求的函数
    void send_request(int64_t a, int64_t b)
    {
        // 【不同点2：死等老板接电话】
        // 因为老板可能还没开机，这里必须等，直到服务端上线
        while (!client_->wait_for_service(std::chrono::seconds(1))) {
            RCLCPP_INFO(this->get_logger(), "等待服务端上线...");
        }

        // 造一个请求包裹（把 a 和 b 装进去）
        auto request = std::make_shared<demo_interfaces::srv::AddTwoInts::Request>();
        request->a = a;
        request->b = b;

        // 【不同点3：发送请求，并等结果】
        // async_send_request 是异步发送，发完不会卡死在这
        // 等老板回电话了，会自动执行后面的 lambda 函数（回调）
        auto future = client_->async_send_request(
            request,
            [this](rclcpp::Client<demo_interfaces::srv::AddTwoInts>::SharedFuture future) {
                // 拿到老板返回的结果
                auto response = future.get();
                RCLCPP_INFO(this->get_logger(), "收到结果！sum = %ld", response->sum);
            }
        );
    }

private:
    // 客户端指针（和服务端的 server_ 对应）
    rclcpp::Client<demo_interfaces::srv::AddTwoInts>::SharedPtr client_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<AddClient>();
    
    // 我们主动呼叫！计算 88 + 12
    node->send_request(88, 12);

    // 原地待命，等待回调函数执行完毕
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
