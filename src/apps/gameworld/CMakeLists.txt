cmake_minimum_required(VERSION 2.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_BUILD_TYPE STREQUAL Release) 
	set(MyProjectName gameworld)
else()
	set(CMAKE_BUILD_TYPE Debug)
	set(MyProjectName gameworldd)
endif()

message(STATUS "CMAKE_BINARY_DIR:${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE}")
message(STATUS "ProjectName:${MyProjectName}")

project(${MyProjectName})

aux_source_directory(. SRCS)
aux_source_directory(../../libserver SRCS)

include_directories(../../libserver)
include_directories(/usr/local/include)
include_directories(${CMAKE_BINARY_DIR}/../../../libs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../../bin")

# set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -pthread -lprotobuf -lcurl -ljsoncpp -lyaml-cpp -llog4cplus -lluajit -DEPOLL")
set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -pthread -DEPOLL")

set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

link_directories(${CMAKE_BINARY_DIR}/../../../libs/linux)
link_directories(${CMAKE_BINARY_DIR}/../../../libs)
link_directories(/usr/lib64)

add_executable(${MyProjectName} ${SRCS})
target_link_libraries(${MyProjectName} -lpthread -lz -lm -lssl -lcrypto -ldl -llua -llog4cplus) # mysql
