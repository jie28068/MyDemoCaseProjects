#include <memory>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/status.h>
#include "../proto/device.grpc.pb.h" // 假设这是由protoc生成的grpc服务头文件
#include "../proto/device.pb.h"      // 假设这是由protoc生成的消息头文件
using namespace device_service;

class DeviceRpcServer final : public DeviceService::Service
{
public:
    DeviceRpcServer() = default;
    grpc::Status checkDevice(grpc::ServerContext *context, auto *request, auto *response);
    grpc::Status GetDeviceStringList(grpc::ServerContext *context, const DeviceNameListRequest *request, DeviceNameListResponse *response);
    grpc::Status GetDeviceSlaveCnt(grpc::ServerContext *context, const DeviceSlaveCntRequest *request, DeviceSlaveCntResponse *response);
    grpc::Status GetDeviceInfo(grpc::ServerContext *context, const DeviceInfoRequest *request, DeviceInfoResponse *response);
    grpc::Status GetDeviceTableBySlaveId(grpc::ServerContext *context, const DeviceTableBySlaveIdRequest *request, DeviceTableBySlaveIdResponse *response);

private:
    std::map<std::string, bool> _deviceMap = {
        {"PCS1", true},
        {"PCS2", true},
        {"BMS1", true},
        {"BMS2", true},
    };
};