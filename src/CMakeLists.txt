add_library(hashing common.h DiskManager.hpp Record.hpp Bucket.hpp HashingScheme.hpp StaticHashing.hpp NaiveScheme.hpp ExtendibleHashing.hpp)
target_include_directories(hashing PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(hashing PUBLIC cereal)
set_target_properties(hashing PROPERTIES LINKER_LANGUAGE CXX)
