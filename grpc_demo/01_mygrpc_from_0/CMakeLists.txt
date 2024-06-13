
cmake_minimum_required(VERSION 3.8)

project(mygrpc CXX)

#include_directories(/root/.local/include)

# 2 find_package come from common.cmake
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)


add_library(my_grpc_lib myhiservice.grpc.pb.cc myhiservice.grpc.pb.h myhiservice.pb.cc myhiservice.pb.h)

# if use common.cmake, ${_REFLECTION} are OK.
#target_link_libraries(my_grpc_lib ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} )
target_link_libraries(my_grpc_lib gRPC::grpc++_reflection gRPC::grpc++ protobuf::libprotobuf)

foreach(_target myclient myserver)
  add_executable(${_target} "${_target}.cpp")
  #target_link_libraries(${_target} my_grpc_lib ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} )
  target_link_libraries(${_target} my_grpc_lib gRPC::grpc++_reflection gRPC::grpc++ protobuf::libprotobuf )
endforeach()
