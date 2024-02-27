cmake_minimum_required(VERSION 3.1.0)
project(SharedControlHardwareInterface VERSION 1.0.0 LANGUAGES C CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
  add_compile_options(-Wall -Wextra -Werror=conversion -Werror=unused-but-set-variable -Werror=return-type)
endif()

find_package(GTest REQUIRED)
find_package(Boost REQUIRED)

