#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <cmath>
#include <chrono>

class LidarNavigator : public rclcpp::Node
{
public:
  LidarNavigator() : Node("navigator"), current_x_(0.0), current_y_(0.0), current_yaw_(0.0), goal_reached_(false)
  {
    this->declare_parameter("goal_x", 5.0);
    this->declare_parameter("goal_y", 5.0);

    scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan", 10, std::bind(&LidarNavigator::scan_callback, this, std::placeholders::_1));

    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
        "/odom", 10, std::bind(&LidarNavigator::odom_callback, this, std::placeholders::_1));

    cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(100), std::bind(&LidarNavigator::control_loop, this));
  }

private:
  void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
  {
    current_x_ = msg->pose.pose.position.x;
    current_y_ = msg->pose.pose.position.y;
    double qz = msg->pose.pose.orientation.z;
    double qw = msg->pose.pose.orientation.w;
    current_yaw_ = 2.0 * std::atan2(qz, qw);
  }

  void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
  {
    avoidance_mag_ = 0.0;
    path_blocked_ = false;

    int center = msg->ranges.size() / 2;
    int window = 30; // Look 40 degrees left and right (80 total)

    for (int i = -window; i < window; ++i)
    {
      float dist = msg->ranges[center + i];
      if (dist < 1)
      { // Safety buffer
        // If something is dead-center and very close, we are blocked
        if (std::abs(i) < 15 && dist < 0.7)
          path_blocked_ = true;

        // Repulsive force math:
        // Closer objects (1/dist) + side bias (i) = turn away
        avoidance_mag_ -= (1.5 / dist) * (i / (double)window);
      }
    }
  }

  void control_loop()
  {
    if (goal_reached_)
      return;

    auto cmd = geometry_msgs::msg::Twist();
    double gx = this->get_parameter("goal_x").as_double();
    double gy = this->get_parameter("goal_y").as_double();

    double dx = gx - current_x_;
    double dy = gy - current_y_;
    double dist_to_goal = std::sqrt(dx * dx + dy * dy);

    // 1. DYNAMIC SUCCESS THRESHOLD
    // Bump this slightly to 0.4 if your 4WD robot is physically large
    if (dist_to_goal < 0.4)
    {
      RCLCPP_INFO(this->get_logger(), "GOAL REACHED!");
      goal_reached_ = true;
      cmd_pub_->publish(cmd);
      return;
    }

    double angle_to_goal = std::atan2(dy, dx);
    double heading_error = angle_to_goal - current_yaw_;
    while (heading_error > M_PI)
      heading_error -= 2.0 * M_PI;
    while (heading_error < -M_PI)
      heading_error += 2.0 * M_PI;

    if (path_blocked_)
    {
      cmd.linear.x = 0.0; // Stop and pivot if blocked
      cmd.angular.z = 0.8;
    }
    else
    {
      // 2. ADAPTIVE SPEED (The Fix)
      // If we are close (< 1.5m), slow down to 0.15 m/s to allow sharper turns
      if (dist_to_goal < 1.5)
      {
        cmd.linear.x = 0.15;
        cmd.angular.z = heading_error * 2.0; // More aggressive steering
      }
      else
      {
        cmd.linear.x = 0.4; // Normal cruise speed
        cmd.angular.z = (heading_error * 0.7) + (avoidance_mag_ * 1.2);
      }
    }
    cmd_pub_->publish(cmd);
  }

  double current_x_, current_y_, current_yaw_, avoidance_mag_;
  bool goal_reached_, path_blocked_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LidarNavigator>());
  rclcpp::shutdown();
  return 0;
}