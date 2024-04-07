cmake_minimum_required(VERSION 3.20)

project(myserver)

set(SRC_LIST main/main.cpp)

# 必须在 add_executable 之上，且 名字 必须放到 add_executable 中
aux_source_directory(common SRC_LIST2)
aux_source_directory(http SRC_LIST2)
# aux_source_directory(main SRC_LIST)
aux_source_directory(net SRC_LIST)
aux_source_directory(thread SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${SRC_LIST2})

# target_include_directories(myserver PUBLIC ${PROJECT_SOURCE_DIR}/include)

# aux_source_directory(common SRC_COMMON)
# aux_source_directory(http SRC_HTTP)
# aux_source_directory(main SRC_MAIN)
# aux_source_directory(net SRC_NET)
# aux_source_directory(thread SRC_THREAD)


# install(TARGETS myserver RUNTIME DESTINATION bin)

# add_subdirectory(common)      // does not contain a CMakeLists.txt file. ...尴尬
# add_subdirectory(http)
# add_subdirectory(main)
# add_subdirectory(net)
# add_subdirectory(thread)
