

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "myhiservice.grpc.pb.h"

class MyHiServiceImpl final: public mymsgpkg::MyHiService::Service {
  grpc::Status SendHi(grpc::ServerContext* ctx, const mymsgpkg::HiRequest* req, mymsgpkg::HiResponse* resp) override {
    resp->set_resp("hi from my server, i got: " + req->req());
    return grpc::Status::OK;
  }
};

void runServer() {
  
  std::string ipport = "0.0.0.0:12345";
  
  MyHiServiceImpl mhs;
  
  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();

  grpc::ServerBuilder builder;
  builder.AddListeningPort(ipport, grpc::InsecureServerCredentials());
  builder.RegisterService(&mhs);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout<<"server listen on "<<ipport<<std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  runServer();
  return 0;
}
