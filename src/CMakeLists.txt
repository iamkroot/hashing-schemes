add_library(hashing common.h DiskManager.hpp Record.hpp Bucket.hpp)
target_include_directories(hashing PUBLIC "${PROJECT_SOURCE_DIR}/src" ${cereal_SOURCE_DIR}/include)
set_target_properties(hashing PROPERTIES LINKER_LANGUAGE CXX)
