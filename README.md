# Project Echo Wave

This project introduces an example utilising Google's Protocol Buffers ([protobuf](https://protobuf.dev/)) with the file `echoWave.proto`. The primary objective is to enhance the project by incorporating two additional binaries, one serving as a client and the other as a server. These binaries will facilitate communication over either TCP or UDP, utilising configurable parameters such as IPv4 address and port, which can be specified either as defined variables or through command line parameters.

## The Server

The server component is tasked with listening on the configured port for incoming connections from the client. Upon connection, it should expect to receive an `echoWaveRequest` encapsulated in an `echoWave` message. Subsequently, the server is required to respond to the client with an `echoWaveResponse` also encapsulated in an `echoWave` message. The echo field in the `echoWaveResponse` should mirror the content of the wave field in the received `echoWaveRequest`, and the status field should be appropriately set.

## The Client

The client component is responsible for establishing a connection to a specified socket, with the socket configuration being customisable either through code or command line parameters. Additionally, the client should accept a string parameter, which will populate the wave field in the `echoWaveRequest`. Upon sending the request, the client must await the `echoWaveResponse` from the server and verify the `status` and the echo field in the response matches the originally sent string.

Before proceeding, ensure that your development environment is equipped with the necessary dependencies: `libc++`, `libprotobuf`, `cmake`, `make`, and `protoc`.


## Build steps

```
mkdir build
cd build
cmake ..
make
```
After successful compilation, you will find a binary named `projectEchoWave_example` within the `build/example` directory.

# Add your own mark

If you get time feel free to infuse your creativity into this project. Elevate the challenge by incorporating encryption techniques, exploring alternative transport layers for seamless Inter-Process Communication (IPC), or even delve into hardware integration for communication across physical serial ports. However, maintain the project's comprehensiveness, ensuring we can replicate your work without hindrance, as we might not possess the exact development board you're using or can't find that niche library you introduced. Let your innovation shine while keeping the project accessible for widespread adoption.
