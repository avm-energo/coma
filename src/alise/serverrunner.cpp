#include "serverrunner.h"

#include <QMutexLocker>
#include <grpcpp/ext/channelz_service_plugin.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
namespace runner
{
void ServerRunner::runServer()
{
    _mutex.lock();
    std::string server_address(m_addr + ":" + std::to_string(m_port));
    SynchronizerServiceImpl service(m_timer.get(), this);
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    grpc::channelz::experimental::InitChannelzService();
    ServerBuilder builder;
    // grpc::ChannelzService
    // builder.
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    m_server = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    _mutex.unlock();
    m_server->Wait();
}

void ServerRunner::stopServer()
{
    QMutexLocker locker(&_mutex);
    Q_CHECK_PTR(m_server);
    gpr_timespec deadline { 60, 0, GPR_CLOCK_MONOTONIC };
    m_server->Shutdown(deadline);
}
}
