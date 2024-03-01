cmake_minimum_required(VERSION 3.1.0)
project(shared_control_hardware_interface_tests VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
  add_compile_options(-Wall -Wextra -Werror=conversion -Werror=unused-but-set-variable -Werror=return-type)
endif()

find_package(GTest REQUIRED)
find_package(SharedControlHardwareInterface REQUIRED)

add_subdirectory(ipc_handler_test)