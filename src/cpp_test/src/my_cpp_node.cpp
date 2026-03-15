#include "rclcpp/rclcpp.hpp"             // The core ROS 2 C++ library
#include "geometry_msgs/msg/twist.hpp"  // The message type

using namespace std::chrono_literals;

class HexagonDriver : public rclcpp::Node {
public:
    HexagonDriver() : Node("hexagon_driver_cpp") {
        // 1. Create Publisher
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

        // 2. Use a Timer instead of 'sleep'
        // This calls 'timer_callback' every 500ms
        timer_ = this->create_wall_timer(500ms, std::bind(&HexagonDriver::timer_callback, this));
    }

private:
    void timer_callback() {
        auto message = geometry_msgs::msg::Twist();
        message.linear.x = 2.0;
        message.angular.z = 1.0; 
        
        RCLCPP_INFO(this->get_logger(), "Publishing velocity...");
        publisher_->publish(message);
    }

    // Member variables (Pointers)
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    // 'Spin' keeps the node alive and processing the timer
    rclcpp::spin(std::make_shared<HexagonDriver>());
    rclcpp::shutdown();
    return 0;
}