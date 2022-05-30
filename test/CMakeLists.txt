add_executable(test_p2d2 test_p2d2.c)
target_link_libraries(test_p2d2 PUBLIC p2d2_lib m)
add_test(NAME p2d2 COMMAND test_p2d2)
