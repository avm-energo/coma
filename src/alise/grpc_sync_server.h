#pragma once

#include "../gen/stdfunc.h"
#include "grpc_proto.grpc.pb.h"

#include <QTimer>
#include <grpcpp/grpcpp.h>

namespace runner
{

using alise::Health;
using alise::HelloReply;
using alise::HelloRequest;
using alise::PowerStatus;
using alise::Reply;
using alise::Synchronizer;
using alise::UnixTimestamp;
using google::protobuf::Empty;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

class ServerRunner;

// Logic and data behind the server's behavior.
class SynchronizerServiceImpl final : public Synchronizer::Service
{
public:
    SynchronizerServiceImpl(QTimer *timer, ServerRunner *parent)
        : Synchronizer::Service(), m_timer(timer), m_parent(parent)
    {
    }

private:
    Status SayHello(ServerContext *context, const HelloRequest *request, HelloReply *reply) override;
    Status GetHealth(ServerContext *context, const Empty *request, Health *health) override;
    Status SetHealth(ServerContext *context, const Health *request, Reply *response) override;
    Status SetTime(ServerContext *context, const UnixTimestamp *request, Reply *response) override;

    Status GetPowerStatus(ServerContext *context, const Empty *request, PowerStatus *response) override;

    // Is this necessary?
    Status GetTime(ServerContext *context, const Empty *request, UnixTimestamp *response) override;

private:
    Health m_health;
    UnixTimestamp m_timestamp;
    QTimer *m_timer;
    ServerRunner *m_parent;
};

}
