#include <memory>
#include <thread>
#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto const node = std::make_shared<rclcpp::Node>(
      "moveit_interface",
      rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  auto spinner = std::thread([&executor]() { executor.spin(); });

  using moveit::planning_interface::MoveGroupInterface;
  std::vector<std::string> group_names = {"left_arm", "right_arm", "left_leg", "right_leg"};

  for (const auto& group_name : group_names) {
    std::cout << "Processing group: " << group_name << std::endl;
    MoveGroupInterface move_group_interface(node, group_name);

    // Get current pose
    geometry_msgs::msg::Pose current_pose = move_group_interface.getCurrentPose().pose;
    std::cout << "Current pose:" << std::endl;
    std::cout << "Position - x: " << current_pose.position.x << ", y: " << current_pose.position.y << ", z: " << current_pose.position.z << std::endl;
    std::cout << "Orientation - x: " << current_pose.orientation.x << ", y: " << current_pose.orientation.y << ", z: " << current_pose.orientation.z << ", w: " << current_pose.orientation.w << std::endl;

    // Set target pose (move up 5cm along z axis)
    geometry_msgs::msg::Pose target_pose = current_pose;
    target_pose.position.z += 0.05;
    move_group_interface.setPoseTarget(target_pose);

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    if (move_group_interface.plan(plan)) {
      std::cout << "Plan for " << group_name << " succeeded, executing..." << std::endl;
      move_group_interface.execute(plan);
    } else {
      std::cout << "Plan for " << group_name << " failed." << std::endl;
    }
  }

  rclcpp::shutdown();
  spinner.join();
  return 0;
}