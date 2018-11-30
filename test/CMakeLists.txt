
add_executable(unit_tests shared_ptr_tests.cpp)
target_link_libraries(unit_tests gtest_main)


add_test(NAME unit COMMAND ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DIR}/unit_tests)