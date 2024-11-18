#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../proto/device.grpc.pb.h" // 由protoc生成的grpc服务头文件
#include "../proto/device.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace device_service;

class DeviceServiceClient
{
public:
    DeviceServiceClient(std::shared_ptr<Channel> channel)
        : _stub(DeviceService::NewStub(channel)) {}

    std::vector<std::string> GetDeviceStringList();
    int GetDeviceSlaveCnt(const std::string &deviceName);
    std::map<std::string, std::string> GetDeviceInfo(const std::string &deviceName);
    std::vector<std::vector<std::string>> GetDeviceTableBySlaveId(const std::string &deviceName, int slaveId, std::string pointName);

private:
    std::unique_ptr<DeviceService::Stub> _stub;
};
