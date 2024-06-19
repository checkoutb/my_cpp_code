

cmake_minimum_required(VERSION 3.18)

project(mongodb_demo CXX)

add_executable(main main.cpp)


find_package(bsoncxx CONFIG REQUIRED)
target_link_libraries(main PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)


find_package(mongocxx CONFIG REQUIRED)
target_link_libraries(main PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)

