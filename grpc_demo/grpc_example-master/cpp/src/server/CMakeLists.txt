set(TARGET device_rpc_server)

# rpc服务端
file(GLOB_RECURSE RPC_SERVER_SOURCES "${CMAKE_SOURCE_DIR}/src/server/*.cpp" "${CMAKE_SOURCE_DIR}/src/proto/*.cc")

# 查找 gRPC 和 Protobuf  
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# 获取版本
message(STATUS "Using protobuf-${Protobuf_VERSION}")
message(STATUS "Using gRPC-${gRPC_VERSION}")

# 包含 gRPC 和 Protobuf 的头文件  
include_directories(${GRPC_INCLUDE_DIRS} ${PROTOBUF_INCLUDE_DIRS})

add_executable(${TARGET} ${RPC_SERVER_SOURCES})

target_include_directories(${TARGET} PUBLIC ${GTEST_INCLUDE_DIRS})
target_link_libraries(${TARGET} PUBLIC gtest gtest_main)
target_link_libraries(${TARGET} PUBLIC gRPC::grpc++_reflection protobuf::libprotobuf)
