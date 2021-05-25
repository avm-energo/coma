#include "grpc_sync_server.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
namespace runner
{

grpc::Status SynchronizerServiceImpl::SayHello(
    grpc::ServerContext *context, const alise::HelloRequest *request, alise::HelloReply *reply)
{
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
}

grpc::Status SynchronizerServiceImpl::GetHealth(
    grpc::ServerContext *context, const Empty *request, alise::Health *health)
{
    return Status::OK;
}

grpc::Status SynchronizerServiceImpl::SetHealth(
    grpc::ServerContext *context, const alise::Health *request, alise::Reply *response)
{
    if (m_timer)
    {
        m_timer->stop();
        m_timer->start();
    }
    return Status::OK;
}

grpc::Status SynchronizerServiceImpl::SetTime(
    grpc::ServerContext *context, const alise::UnixTimestamp *request, alise::Reply *response)
{
    return Status::OK;
}

grpc::Status SynchronizerServiceImpl::GetTime(
    grpc::ServerContext *context, const Empty *request, alise::UnixTimestamp *response)
{
    return Status::OK;
}

static unsigned int stoui(const std::string &s)
{
    unsigned long lresult = stoul(s, nullptr, 10);
    uint16_t result = lresult;
    if (result != lresult)
        throw std::out_of_range("Not a uint_16");
    return result;
}

void ServerRunner::RunServer()
{
    std::string server_address(m_server + ":" + std::to_string(m_port));
    SynchronizerServiceImpl service(m_timer.get());

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

}
