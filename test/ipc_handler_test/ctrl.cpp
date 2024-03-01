/**
 * @file ctrl.cpp
 * @author enaciodabasi@outlook.com
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

  ipc_handler::hardware_interface::IpcHandlerHWI hwiShmHandler = ipc_handler::hardware_interface::IpcHandlerHWI();
  
  auto initRes = hwiShmHandler.init("/home/naci/shared_control_hardware_interface/test/ipc_handler_test/config.yaml");
  
  EXPECT_EQ(initRes, ReturnCode::Success);

  /* auto setOpRes = hwiShmHandler.setSharedObjectVariable(
    "left_wheel",
    joint::ActualVelocity,
    1.0
  );

  EXPECT_EQ(setOpRes, ReturnCode::Success);

  auto actVelOpt = hwiShmHandler.getSharedObjectVariable(
    "left_wheel",
    joint::JointVariables::TargetVelocity
  );

  EXPECT_NE(actVelOpt, std::nullopt);
 */
  /* if(actVelOpt)
    std::cout << actVelOpt.value() << std::endl; */

  bool runLoop = true;

  std::signal(SIGINT, signal_handler);

  shutdown_handler = [&runLoop](int signal)
  {
    runLoop = false;
  };

  while (runLoop)
  {

    auto targetVel = hwiShmHandler.getSharedObjectVariable("left_wheel", joint::JointVariables::TargetVelocity);
    if(!targetVel)
    {
      break;
    }

    std::cout << "Target Velocity: " << targetVel.value() << std::endl;

    hwiShmHandler.setSharedObjectVariable(
      "left_wheel",
      joint::JointVariables::ActualVelocity,
      targetVel.value() + 0.5
    );

    std::this_thread::sleep_for(4ms);

  }
  

}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

