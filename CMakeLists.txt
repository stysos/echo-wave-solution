cmake_minimum_required(VERSION "3.0")
project("projectEchoWave")

set(ECHO_WAVE_LIB "echoWaveProtoBuf")
set(PROTOBUF_DIR "protobuf")
set(EXAMPLE_DIR "example")
set(SOLUTION_DIR "sam-solution")


# Create a Library for the echoWave.proto
find_package(Protobuf REQUIRED) # The host machine should have google protobug lib and protoc installed
include_directories(${Protobuf_INCLUDE_DIRS}) # Include the google protobuf headers
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # This is where the Proto files are generated so include them
set (ECHO_WAVE_PROTO "${PROTOBUF_DIR}/echoWave.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${ECHO_WAVE_PROTO})
add_library(${ECHO_WAVE_LIB} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(${ECHO_WAVE_LIB} ${Protobuf_LIBRARIES})
target_include_directories (${ECHO_WAVE_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# Inclue the example dir so it gets built
add_subdirectory("${EXAMPLE_DIR}")
add_subdirectory("${SOLUTION_DIR}")

