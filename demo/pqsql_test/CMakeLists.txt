cmake_minimum_required(VERSION 3.0)
project(test_pg)
# find_package(PostgreSQL REQUIRED)
set(CMAKE_CXX_STANDARD 11)

add_executable(test_pg src/pgtest.cc)
target_include_directories(test_pg PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(test_pg glog ${PostgreSQL_LIBRARIES})
