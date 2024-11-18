import time
from typing import List, Dict

import grpc

import sys
sys.path.append("..")

from proto import device_pb2
from proto import device_pb2_grpc


class DeviceServiceClient:
    def __init__(self, channel_address='localhost:50051'):
        self.channel = grpc.insecure_channel(channel_address)
        self.stub = device_pb2_grpc.DeviceServiceStub(self.channel)

    def getDeviceStringList(self) -> List[str]:
        device_names_response = self.stub.GetDeviceStringList(device_pb2.DeviceNameListRequest())
        device_names = [name for name in device_names_response.device_names]
        return device_names

    def getDeviceInfo(self, device_name: str) -> Dict:
        device_info_response = self.stub.GetDeviceInfo(device_pb2.DeviceInfoRequest(device_name=device_name))
        device_info_dict = {
            "ip": device_info_response.info.ip,
            "port": device_info_response.info.port,
            "type": device_info_response.info.type,
            "server_status": device_info_response.info.server_status,
            "simulate_status": device_info_response.info.simulate_status,
            "plan_status": device_info_response.info.plan_status
        }
        return device_info_dict

    def getDeviceSlaveCnt(self, device_name: str) -> int:
        device_slave_cnt_response = self.stub.GetDeviceSlaveCnt(
            device_pb2.DeviceSlaveCntRequest(device_name=device_name))
        slave_cnt = device_slave_cnt_response.slave_cnt
        return slave_cnt

    def getDeviceTableBySlaveId(self, device_name: str, slave_id: int, point_name: str = "") -> Dict:
        response = self.stub.GetDeviceTableBySlaveId(
            device_pb2.DeviceTableBySlaveIdRequest(device_name=device_name, slave_id=slave_id, point_name=point_name))
        head_data = [head for head in response.head_data.row]
        # 处理响应中的二维数组数据
        table_data = []
        for value in response.table_data:
            row_data = [item for item in value.row]
            table_data.append(row_data)
        data_dict = {
            "head_data": head_data,
            "table_data": table_data
        }
        return data_dict

    def close(self):
        self.channel.close()


if __name__ == "__main__":
    client = DeviceServiceClient()

    device_name_list = client.getDeviceStringList()
    print(device_name_list)

    device_info_dict = client.getDeviceInfo("PCS1")
    print(device_info_dict)

    slave_cnt = client.getDeviceSlaveCnt("PCS1")
    print(slave_cnt)

    table_data_dict = client.getDeviceTableBySlaveId("PCS1", 1, "")
    print(table_data_dict)

    client.close()