add_executable(tests disk_manager_test.cpp bucket_test.cpp)
target_link_libraries(tests PRIVATE hashing)
