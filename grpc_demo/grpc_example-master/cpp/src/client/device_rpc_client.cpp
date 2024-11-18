#include "device_rpc_client.h"

std::vector<std::string> DeviceServiceClient::GetDeviceStringList()
{
    ClientContext context;
    DeviceNameListRequest request;
    DeviceNameListResponse response;

    Status status = _stub->GetDeviceStringList(&context, request, &response);

    if (status.ok())
    {
        std::vector<std::string> device_names;
        for (const auto &name : response.device_names())
        {
            device_names.push_back(name);
        }
        return device_names;
    }
    else
    {
        std::cerr << "GetDeviceStringList Failed: " << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return {};
    }
}

int DeviceServiceClient::GetDeviceSlaveCnt(const std::string &deviceName)
{
    DeviceSlaveCntRequest request;
    request.set_device_name(deviceName);
    DeviceSlaveCntResponse response;
    ClientContext context;
    Status status = _stub->GetDeviceSlaveCnt(&context, request, &response);
    if (status.ok())
    {
        int slaveCnt = response.slave_cnt();
        return slaveCnt;
    }
    else
    {
        std::cerr << "GetDeviceSlaveCnt Failed:" << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return -1;
    }
}

std::map<std::string, std::string> DeviceServiceClient::GetDeviceInfo(const std::string &deviceName)
{
    DeviceInfoRequest request;
    request.set_device_name(deviceName);
    DeviceInfoResponse response;
    ClientContext context;
    Status status = _stub->GetDeviceInfo(&context, request, &response);
    if (status.ok())
    {
        std::map<std::string, std::string> infoMap;
        infoMap["ip"] = response.info().ip();
        infoMap["port"] = std::to_string(response.info().port());
        infoMap["type"] = response.info().type();
        infoMap["server_status"] = std::to_string(response.info().server_status());
        infoMap["simulate_status"] = std::to_string(response.info().simulate_status());
        infoMap["plan_status"] = std::to_string(response.info().plan_status());
        return infoMap;
    }
    else
    {
        std::cerr << "GetDeviceInfo Failed: " << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return {};
    }
}

std::vector<std::vector<std::string>> DeviceServiceClient::GetDeviceTableBySlaveId(const std::string &deviceName, int slaveId, std::string pointName)
{
    DeviceTableBySlaveIdRequest request;
    DeviceTableBySlaveIdResponse response;
    ClientContext context;
    request.set_device_name(deviceName);
    request.set_slave_id(slaveId);
    request.set_point_name(pointName);

    Status status = _stub->GetDeviceTableBySlaveId(&context, request, &response);
    if (status.ok())
    {
        std::vector<std::vector<std::string>> table;

        // 处理列表头数据
        DeviceTableRow headRow = response.head_data();
        std::vector<std::string> head;
        for (const std::string &value : headRow.row())
        {
            head.push_back(value);
        }
        table.push_back(head);

        // 处理列表数据
        for (int i = 0; i < response.table_data().size(); i++)
        {
            DeviceTableRow tableRow = response.table_data().Get(i);
            std::vector<std::string> row;
            for (const std::string &value : tableRow.row())
            {
                row.push_back(value);
            }
            table.push_back(row);
        }
        return table;
    }
    else
    {
        std::cout << "GetDeviceTableBySlaveId error: " << status.error_message() << std::endl;
        return {};
    }
}