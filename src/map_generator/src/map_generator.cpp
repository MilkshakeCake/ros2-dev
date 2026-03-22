#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"

void generate_world(const std::string &path, double gx, double gy)
{
    std::ofstream world_file(path);
    if (!world_file.is_open())
    {
        std::cerr << "Could not open file for writing!" << std::endl;
        return;
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine gen(seed);
    std::uniform_real_distribution<double> dist(-4.0, 4.0);

    world_file << "<?xml version='1.0'?>\n<sdf version='1.6'>\n  <world name='default'>\n";
    world_file << "    <include><uri>model://ground_plane</uri></include>\n";
    world_file << "    <include><uri>model://sun</uri></include>\n";

    // VIRTUAL FLAG (Non-collisional)
    world_file << "    <model name='virtual_flag'>\n"
               << "      <static>true</static>\n"
               << "      <pose>" << gx << " " << gy << " 0 0 0 0</pose>\n"
               << "      <link name='link'>\n"
               << "        <visual name='v'><geometry><cylinder><radius>0.05</radius><length>1.5</length></cylinder></geometry>"
               << "        <material><ambient>1 0 0 1</ambient><diffuse>1 0 0 1</diffuse></material></visual>\n"
               << "      </link>\n"
               << "    </model>\n";

    // RANDOM OBSTACLES
    int count = 0;
    while (count < 12)
    {
        double x = dist(gen);
        double y = dist(gen);

        double dist_to_start = std::sqrt(x * x + y * y);
        double dist_to_goal = std::sqrt(std::pow(x - gx, 2) + std::pow(y - gy, 2));

        if (dist_to_start > 1.2 && dist_to_goal > 1.0)
        {
            world_file << "    <model name='obs_" << count << "'>\n"
                       << "      <static>true</static>\n"
                       << "      <pose>" << x << " " << y << " 0.5 0 0 0</pose>\n"
                       << "      <link name='link'>\n"
                       << "        <collision name='c'><geometry><box><size>0.5 0.5 1.0</size></box></geometry></collision>\n"
                       << "        <visual name='v'><geometry><box><size>0.5 0.5 1.0</size></box></geometry></visual>\n"
                       << "      </link>\n"
                       << "    </model>\n";
            count++;
        }
    }

    world_file << "  </world>\n</sdf>";
    world_file.close();
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("map_generator");

    node->declare_parameter("goal_x", 5.0);
    node->declare_parameter("goal_y", 5.0);

    double gx = node->get_parameter("goal_x").as_double();
    double gy = node->get_parameter("goal_y").as_double();

    std::string path = ament_index_cpp::get_package_share_directory("map_generator") + "/random_map.world";

    generate_world(path, gx, gy);

    RCLCPP_INFO(node->get_logger(), "World generated at: %s", path.c_str());
    rclcpp::shutdown();
    return 0;
}