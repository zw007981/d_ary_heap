cmake_minimum_required(VERSION 3.16)
project(priority-queue)
set(CMAKE_CXX_STANDARD 17)

# 开启缓存预编译。
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "在priority-queue中开启缓存预编译。")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

# 添加使用示例。
add_executable(run_priority_queue_example main.cpp)

# 运行单元测试。
find_package(GTest)
if(GTest_FOUND)
  enable_testing()
  include(GoogleTest)
  file(GLOB TEST_SRC CONFIGURE_DEPENDS "test/*.t.cpp")
  add_executable(test_priority_queue ${TEST_SRC})
  target_link_libraries(test_priority_queue gmock gtest pthread)
  gtest_discover_tests(test_priority_queue XML_OUTPUT_DIR
                       ${CMAKE_CURRENT_BINARY_DIR}/Testing)
else(GTest_FOUND)
  message("Warning: Unable to find GTest!")
endif(GTest_FOUND)
