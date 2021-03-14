add_executable(tests disk_manager_test.cpp bucket_test.cpp common.hpp static_hashing_test.cpp Stopwatch.hpp)
target_link_libraries(tests PRIVATE hashing)
