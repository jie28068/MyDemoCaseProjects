#include <memory>
#include <grpcpp/grpcpp.h>
#include "device_rpc_server.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

void runServer()
{
    std::string server_address("0.0.0.0:50051");
    DeviceRpcServer rpcServer;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&rpcServer);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main()
{
    runServer();
    return 0;
}