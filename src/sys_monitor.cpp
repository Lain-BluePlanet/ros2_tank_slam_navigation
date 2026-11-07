#include "rclcpp/rclcpp.hpp"
#include "demo_interfaces/msg/sys_status.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>

using namespace std::chrono_literals;

class SysMonitor : public rclcpp::Node
{
public:
    SysMonitor() : Node("sys_monitor_node")
    {
        // 把读到的数据发到这个叫 /sys_status 的频道
        pub_ = this->create_publisher<demo_interfaces::msg::SysStatus>("/sys_status", 10);
        // 每 1 秒偷看一次电脑
        timer_ = this->create_wall_timer(1s, std::bind(&SysMonitor::read_sys, this));
    }

private:
    void read_sys()
    {
        auto msg = demo_interfaces::msg::SysStatus();

        // --- 1. 读 CPU 数据 ---
        std::ifstream stat("/proc/stat");
        std::string line;
        std::getline(stat, line); // 读取第一行（总CPU信息）
        std::istringstream iss(line);
        std::string cpu; 
        long user, nice, system, idle;
        iss >> cpu >> user >> nice >> system >> idle;
        // 简单的数学计算算出占用率
        long total = user + nice + system + idle;
        long idle_diff = idle - last_idle_;
        long total_diff = total - last_total_;
        float cpu_usage = 100.0 * (total_diff - idle_diff) / total_diff;
        last_idle_ = idle; 
        last_total_ = total;
        msg.cpu_percent = cpu_usage;

        // --- 2. 读内存数据 ---
        std::ifstream meminfo("/proc/meminfo");
        long total_mem = 1, avail_mem = 1;
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal:") == 0) { std::istringstream(line) >> line >> total_mem; }
            if (line.find("MemAvailable:") == 0) { std::istringstream(line) >> line >> avail_mem; }
        }
        msg.memory_percent = 100.0 * (total_mem - avail_mem) / total_mem;

        // 把数据发出去！
        pub_->publish(msg);
    }

    rclcpp::Publisher<demo_interfaces::msg::SysStatus>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    long last_idle_ = 0, last_total_ = 0; // 用于算CPU差值
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SysMonitor>());
    rclcpp::shutdown();
    return 0;
}
