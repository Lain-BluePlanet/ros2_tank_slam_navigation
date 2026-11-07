#include "rclcpp/rclcpp.hpp"
// 引入咱们刚造好的自定义服务接口
#include "demo_interfaces/srv/add_two_ints.hpp" 

class AddServer : public rclcpp::Node
{
public:
    AddServer() : Node("add_server_node")
    {
        // 【核心动作1：挂牌营业】
        // 创建一个服务端，服务名字叫 "add_two_ints"
        // 只要有人向这个名字发请求，就去执行 execute_callback 函数
        server_ = this->create_service<demo_interfaces::srv::AddTwoInts>(
            "add_two_ints",
            std::bind(&AddServer::execute_callback, this, std::placeholders::_1, std::placeholders::_2)
        );
        RCLCPP_INFO(this->get_logger(), "加法服务端已启动！等待客户呼叫...");
    }

private:
    // 【核心动作2：处理请求】
    // request 是客户发来的数据（a 和 b）
    // response 是要发还给客户的结果（sum）
    void execute_callback(
        const demo_interfaces::srv::AddTwoInts::Request::SharedPtr request,
        demo_interfaces::srv::AddTwoInts::Response::SharedPtr response)
    {
        // 老板（服务端）开始算账
        response->sum = request->a + request->b;
        
        // 打印一下，让终端里的人看到有人在点外卖
        RCLCPP_INFO(this->get_logger(), "收到请求: a=%ld, b=%ld. 正在计算...", request->a, request->b);
        RCLCPP_INFO(this->get_logger(), "计算完成，返回结果: sum=%ld", response->sum);
    }

    // 定义一个服务端的指针
    rclcpp::Service<demo_interfaces::srv::AddTwoInts>::SharedPtr server_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<AddServer>();
    
    // 原地待命，挂机等客户打电话
    rclcpp::spin(node); 
    
    rclcpp::shutdown();
    return 0;
}
