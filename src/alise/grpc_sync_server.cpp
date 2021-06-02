#include "grpc_sync_server.h"

#include "serverrunner.h"

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
    std::cout << reply->message().c_str() << std::endl;
    return Status::OK;
}

grpc::Status SynchronizerServiceImpl::GetHealth(
    grpc::ServerContext *context, const Empty *request, alise::Health *health)
{
    health->set_code(m_health.code());
    health->set_desc(m_health.desc());
    std::cout << health->SerializeAsString() << std::endl;
    return Status::OK;
}

grpc::Status SynchronizerServiceImpl::SetHealth(
    grpc::ServerContext *context, const alise::Health *request, alise::Reply *response)
{
    m_health.set_code(request->code());
    m_health.set_desc(request->desc());
    std::cout << m_health.SerializeAsString();
    m_parent->healthChanged(*request);
    response->set_status(true);
    return Status::OK;
}

grpc::Status SynchronizerServiceImpl::SetTime(
    grpc::ServerContext *context, const alise::UnixTimestamp *request, alise::Reply *response)
{

    std::cout << response->status() << "," << request;
    m_parent->timeChanged(*request);
    return Status::OK;
}

grpc::Status SynchronizerServiceImpl::GetPowerStatus(
    grpc::ServerContext *context, const google::protobuf::Empty *request, alise::PowerStatus *response)
{
    std::cout << request << std::endl;
    m_parent->requestPowerStatus(*response);
    return Status::OK;
}

grpc::Status SynchronizerServiceImpl::GetTime(
    grpc::ServerContext *context, const Empty *request, alise::UnixTimestamp *response)
{
    std::cout << request << std::endl;
    // alise::UnixTimestamp buffer;
    m_parent->requestTime(*response);
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

}
