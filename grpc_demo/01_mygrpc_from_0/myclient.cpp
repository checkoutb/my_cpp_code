

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "myhiservice.grpc.pb.h"


class HiServiceClient {
public:

  // myhiservice.proto, package mymsgpkg;
 
  HiServiceClient(std::shared_ptr<grpc::Channel> channel) : stub_(mymsgpkg::MyHiService::NewStub(channel)) {}

  std::string sendaHi(const std::string& msg) {
    mymsgpkg::HiRequest req;
    req.set_req(msg);

    mymsgpkg::HiResponse resp;

    grpc::ClientContext ctx;
    grpc::Status status = stub_ -> SendHi(&ctx, req, &resp);

    if (status.ok()) {

      return resp.resp();
    } else {

      std::cout<<"errrrrror. "<<status.error_code()<<": "<<status.error_message()<<std::endl;
      return "gRPC failed..";
    }
  }

private:
  std::unique_ptr<mymsgpkg::MyHiService::Stub> stub_;
};

int main(int argc, char** argv) {
  
  // ip + port
  std::string ipport = "0.0.0.0:12345";

  HiServiceClient hiSC(grpc::CreateChannel(ipport, grpc::InsecureChannelCredentials()));

  std::string str("msssssg from client my.");
  std::string reply = hiSC.sendaHi(str);
  
  std::cout<<"client received: "<<reply<<std::endl;

  return 0;
}


