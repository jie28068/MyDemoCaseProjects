#include "device_rpc_server.h"

grpc::Status DeviceRpcServer::checkDevice(grpc::ServerContext *context, auto *request, auto *response)
{
    std::string device_name = request->device_name();
    auto device = _deviceMap.find(device_name);
    if (device == _deviceMap.end())
    {
        // 处理设备不存在的情况，返回错误状态
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "未找到" + device_name + "设备");
    }
    // 设备存在，返回成功状态
    return grpc::Status::OK;
}

grpc::Status DeviceRpcServer::GetDeviceStringList(grpc::ServerContext *context, const DeviceNameListRequest *request, DeviceNameListResponse *response)
{
    std::vector<std::string> deviceNameList = {"PCS1", "PCS2", "BMS1", "BMS2"};
    for (const auto &name : deviceNameList)
    {
        // 这个函数是grpc自动生成的，名称与.proto文件里面的变量名有关
        response->add_device_names(name);
    }
    return grpc::Status::OK;
}

grpc::Status DeviceRpcServer::GetDeviceSlaveCnt(grpc::ServerContext *context, const DeviceSlaveCntRequest *request, DeviceSlaveCntResponse *response)
{
    auto check = checkDevice(context, request, response);
    if (check.ok())
    {
        int slaveCnt = 1;
        response->set_slave_cnt(slaveCnt);
        return grpc::Status::OK;
    }
    else
    {
        return check;
    }
}

grpc::Status DeviceRpcServer::GetDeviceInfo(grpc::ServerContext *context, const DeviceInfoRequest *request, DeviceInfoResponse *response)
{
    std::string device_name = request->device_name();
    auto check = checkDevice(context, request, response);
    if (check.ok())
    {
        DeviceInfoDetail deviceInfo;
        std::string ip = "127.0.0.1";
        deviceInfo.set_ip(ip);

        int port = 502;
        deviceInfo.set_port(port);

        std::string type = "tcp";
        deviceInfo.set_type(type);

        bool serverStatus = true;
        deviceInfo.set_server_status(serverStatus);

        bool simulatorStatus = true;
        deviceInfo.set_simulate_status(simulatorStatus);

        bool planStatus = false;
        deviceInfo.set_plan_status(planStatus);

        // 将自定义类型DeviceInfo拷贝到返回消息里面去
        response->mutable_info()->CopyFrom(deviceInfo);
        return grpc::Status::OK;
    }
    else
    {
        return check;
    }
}

grpc::Status DeviceRpcServer::GetDeviceTableBySlaveId(grpc::ServerContext *context, const DeviceTableBySlaveIdRequest *request, DeviceTableBySlaveIdResponse *response)
{
    auto check = checkDevice(context, request, response);
    if (check.ok())
    {
        // 构造一组二维列表数据
        std::vector<std::string> headData = {
            "设备名称",
            "设备类型",
            "设备IP",
            "设备端口",
            "设备状态",
            "模拟状态",
            "计划状态",
        };
        std::vector<std::vector<std::string>> tableData = {
            {"PCS1", "tcp", "127.0.0.1", "502", "true", "true", "false"},
            {"PCS2", "tcp", "127.0.0.1", "503", "true", "true", "false"},
            {"BMS1", "tcp", "127.0.0.1", "504", "true", "true", "false"},
            {"BMS2", "tcp", "127.0.0.1", "505", "true", "true", "false"},
        };
        DeviceTableRow headRow;
        for (const auto &data : headData)
        {
            headRow.add_row(data);
        }
        response->mutable_head_data()->CopyFrom(headRow);

        for (const auto &data : tableData)
        {
            DeviceTableRow table;
            for (const auto &item : data)
            {
                table.add_row(item);
            }
            response->add_table_data()->CopyFrom(table);
        }
        return grpc::Status::OK;
    }
    else
    {
        return check;
    }
}