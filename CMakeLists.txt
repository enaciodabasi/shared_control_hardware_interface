cmake_minimum_required(VERSION 3.1.0)
project(SharedControlHardwareInterface VERSION 1.0.0 LANGUAGES C CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
  add_compile_options(-Wall -Wextra -Werror=conversion -Werror=unused-but-set-variable -Werror=return-type)
endif()

find_package(Boost REQUIRED)
find_package(yaml-cpp REQUIRED)

set(SOURCES
  src/ipc_handler.cpp
  src/joint.cpp
)

add_library(
  ${CMAKE_PROJECT_NAME}
  SHARED
  ${SOURCES}
)
target_compile_features(
  ${CMAKE_PROJECT_NAME}
  PUBLIC cxx_std_17
)
target_include_directories(
  ${CMAKE_PROJECT_NAME}
  PUBLIC
  ${Boost_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIRS}
  include/
)
target_link_libraries(
  ${CMAKE_PROJECT_NAME}
  ${YAML_CPP_LIBRARIES}
  ##${}
)

set(GENERAL_INSTALL_DIR /usr/local)
set(INCLUDE_INSTALL_DIR ${GENERAL_INSTALL_DIR}/include)
set(LIBS_INSTALL_DIR ${GENERAL_INSTALL_DIR}/lib) 

install(
  TARGETS ${CMAKE_PROJECT_NAME}
  ARCHIVE DESTINATION ${LIBS_INSTALL_DIR}/${CMAKE_PROJECT_NAME}
  LIBRARY DESTINATION ${LIBS_INSTALL_DIR}/${CMAKE_PROJECT_NAME}
)

install(
  DIRECTORY include/
  DESTINATION ${INCLUDE_INSTALL_DIR}/${CMAKE_PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
set(INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/${CMAKE_PROJECT_NAME})
set(LIB_INSTALL_DIR ${LIBS_INSTALL_DIR}/${CMAKE_PROJECT_NAME})
set(SH_LIB_FILE_NAME lib${CMAKE_PROJECT_NAME}.so)
set(LIBS ${LIB_INSTALL_DIR}/${SH_LIB_FILE_NAME})

configure_package_config_file(
  ${CMAKE_PROJECT_NAME}.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION /usr/local/lib/cmake/${CMAKE_PROJECT_NAME}
    PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR LIBS
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
    DESTINATION /usr/local/lib/cmake/${CMAKE_PROJECT_NAME})
