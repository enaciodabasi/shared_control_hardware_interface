project(ipc_handler_test)

add_executable(
  hwi_test
  hwi.cpp
)
target_include_directories(
  hwi_test
  PUBLIC
  ${SharedControlHardwareInterface_INCLUDE_DIR}
  ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(
  hwi_test
  ${SharedControlHardwareInterface_LIBS}
  ${GTEST_LIBRARIES}
)

install(
  TARGETS hwi_test
  DESTINATION ${CMAKE_PROJECT_NAME}/build
)

add_executable(
  ctrlr_test
  ctrl.cpp
)
target_include_directories(
  ctrlr_test
  PUBLIC
  ${SharedControlHardwareInterface_INCLUDE_DIR}
  ${GTEST_INCLUDE_DIRS}
)
target_link_libraries(
  ctrlr_test
  ${SharedControlHardwareInterface_LIBS}
  ${GTEST_LIBRARIES}
)