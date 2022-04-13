cmake_minimum_required(VERSION 3.16)
project(priority-queue)
set(CMAKE_CXX_STANDARD 17)

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
    message(STATUS "GTest found, test cases can be executed.")
else(GTest_FOUND)
    message(WARNING "Unable to find GTest!")
endif(GTest_FOUND)
