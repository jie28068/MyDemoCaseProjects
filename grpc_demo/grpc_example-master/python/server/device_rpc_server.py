from concurrent import futures
from typing import Dict

import grpc

import sys
sys.path.append("..")

from proto import device_pb2
from proto import device_pb2_grpc


class DeviceServiceServicer(device_pb2_grpc.DeviceServiceServicer):
    def __init__(self) -> None:
        self.device_map: Dict[str,bool] = {
            "PCS1": True,
            "PCS2": True,
            "BMS1": True,
            "BMS2": True,
        }

    def checkDevice(self, request, context) -> bool:
        device = self.device_map.get(request.device_name)
        if device is None:
            # 处理设备不存在的情况，例如返回一个错误消息或抛出一个自定义异常
            context.set_code(grpc.StatusCode.NOT_FOUND)
            context.set_details(f'未找到{request.device_name}设备')
            print(f'未找到{request.device_name}设备')
            return False
        else:
            return True

    def GetDeviceStringList(self, request, context):
        device_name_list = ["PCS1", "PCS2", "BMS1", "BMS2"]
        return device_pb2.DeviceNameListResponse(device_names=device_name_list)

    def GetDeviceInfo(self, request, context):
        if not self.checkDevice(request, context):
            return device_pb2.DeviceInfoResponse()
        try:
            info_detail = device_pb2.DeviceInfoDetail(
                ip="127.0.0.1",
                port=502,
                type="tcp",
                server_status=True,
                simulate_status=True,
                plan_status=False
            )
            # 创建并返回响应
            response = device_pb2.DeviceInfoResponse(info=info_detail)
            return response
        except Exception as e:
            print(e)
            return device_pb2.DeviceInfoResponse()

    def GetDeviceSlaveCnt(self, request, context):
        if not self.checkDevice(request, context):
            return device_pb2.DeviceSlaveCntResponse(slave_cnt=0)
        try:
            # 创建并返回响应
            response = device_pb2.DeviceSlaveCntResponse(slave_cnt=1)
            return response
        except Exception as e:
            print(e)
            return device_pb2.DeviceSlaveCntResponse(slave_cnt=0)

    def GetDeviceTableBySlaveId(self, request, context):
        if not self.checkDevice(request, context):
            return device_pb2.DeviceTableBySlaveIdResponse(head_data=device_pb2.DeviceTableRow(row=[]),
                                                           table_data=[device_pb2.DeviceTableRow(row=[])])
        try:
            # 创建并返回响应
            head_data = ["设备名称","设备类型","设备IP","设备端口","设备状态","模拟状态","计划状态"]
            table_data = [["PCS1", "tcp", "127.0.0.1", "502", "true", "true", "false"],
                          ["PCS2", "tcp", "127.0.0.1", "503", "true", "true", "false"],
                          ["BMS1", "tcp", "127.0.0.1", "504", "true", "true", "false"],
                          ["BMS2", "tcp", "127.0.0.1", "505", "true", "true", "false"]]
            # 封装成rpc格式
            head_row = device_pb2.DeviceTableRow(row=head_data)
            response = device_pb2.DeviceTableBySlaveIdResponse(
                head_data=head_row,
                table_data=[device_pb2.DeviceTableRow(row=row_data) for row_data in table_data]
            )
            return response
        except Exception as e:
            print(e)
            return device_pb2.DeviceTableBySlaveIdResponse(head_data=device_pb2.DeviceTableRow(row=[]),
                                                           table_data=[device_pb2.DeviceTableRow(row=[])])

def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    device_pb2_grpc.add_DeviceServiceServicer_to_server(DeviceServiceServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    server.wait_for_termination()

if __name__ == '__main__':
    serve()