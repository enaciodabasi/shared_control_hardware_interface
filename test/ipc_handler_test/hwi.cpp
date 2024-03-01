/**
 * @file hwi.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <csignal>
#include <thread>
#include <chrono>

#include <gtest/gtest.h>

#include "shared_control_hardware_interface/ipc_handler.hpp"

namespace {
std::function<void(int)> shutdown_handler;
void signal_handler(int signal) { shutdown_handler(signal); }
} // namespace

using namespace std::chrono_literals;

TEST(HardwareIpcClassTest, InitializationTest)
{
  using namespace schi;

  ipc_handler::controller_interface::IpcHandlerCI ctrlrShmHandler = ipc_handler::controller_interface::IpcHandlerCI();
  
  auto initRes = ctrlrShmHandler.init("/home/naci/shared_control_hardware_interface/test/ipc_handler_test/config.yaml");
  
  EXPECT_EQ(initRes, ReturnCode::Success);

  /* auto setOpRes = ctrlrShmHandler.setSharedObjectVariable(
    "left_wheel",
    joint::TargetVelocity,
    1.0
  );

  EXPECT_EQ(setOpRes, ReturnCode::Success);

  auto actVelOpt = ctrlrShmHandler.getSharedObjectVariable(
    "left_wheel",
    joint::JointVariables::TargetVelocity
  );

  EXPECT_NE(actVelOpt, std::nullopt);

  if(actVelOpt)
    std::cout << actVelOpt.value() << std::endl; */

  bool runLoop = true;

  std::signal(SIGINT, signal_handler);

  shutdown_handler = [&runLoop](int signal)
  {
    runLoop = false;
  };

  double target = 0.0;
  while(runLoop)
  {
    ctrlrShmHandler.setSharedObjectVariable(
      "left_wheel",
      joint::JointVariables::TargetVelocity,
      target
    );

    target += 1.0;

    auto actVel = ctrlrShmHandler.getSharedObjectVariable("left_wheel", joint::JointVariables::ActualVelocity);
    if(!actVel)
    {
      break;
    }

    std::cout << "Actual Velocity: " << actVel.value() << std::endl;

    std::this_thread::sleep_for(4ms);
  }

}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}