#include "client.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <mutex>
#include <thread>

#include "server.hpp"

Client::Client(int port) {
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    std::cerr << "Error creating socket.\n";
    std::exit(EXIT_FAILURE);
  }

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr =
      inet_addr("127.0.0.1");            // Use the server's IP address
  serverAddress.sin_port = htons(port);  // Use the server's port

  if (connect(clientSocket, reinterpret_cast<struct sockaddr *>(&serverAddress),
              sizeof(serverAddress)) == -1) {
    std::cerr << "Error connecting to server. Please check the port\n";
    close(clientSocket);
    std::exit(EXIT_FAILURE);
  }

  std::cout << "Connected to server.\n";
}

void Client::startChat() {
  std::thread(&Client::receiveMessages, this).detach();
  while (true) {
    sendMessage();
  }
}

void Client::sendMessage() {
  std::string message;
  std::cout << "You: ";

  // Keep prompting until a non-empty message is entered
  do {
    std::getline(std::cin, message);
  } while (message.empty());

  // Lock the console output
  {
    std::lock_guard<std::mutex> lock(consoleMutex);
    std::string compiledMessage = createMessage(message);
    send(clientSocket, compiledMessage.c_str(), compiledMessage.length(), 0);
  }

  if (message == "exit") {
    close(clientSocket);
    std::exit(EXIT_SUCCESS);
  }
}

std::string Client::createMessage(std::string waveString) {
  echoWaveRequest request{};
  request.set_wave(waveString);

  echoWave msg{};
  msg.mutable_request()->CopyFrom(request);

  std::string serialisedMessage;
  msg.SerializeToString(&serialisedMessage);

  return serialisedMessage;
}

echoWave Client::decodeMessage(char *message) {
  echoWave msg{};
  if (msg.ParseFromString(message)) {
    if (msg.has_response()) {
      echoWaveResponse response = msg.response();
      return msg;
    } else if (msg.has_request()) {
      echoWaveRequest request = msg.request();
      return msg;
    } else {
      std::cerr << "Unknown Message Type received" << std::endl;
      // Add decode error string
    }
  }
  // Add decode error string
  return msg;
}

void Client::receiveMessages() {
  char buffer[1024];

  while (true) {
    int bytesRead = recv(Client::clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
      std::cerr << "Connection closed by server.\n";
      close(Client::clientSocket);
      std::exit(EXIT_FAILURE);
    }

    buffer[bytesRead] = '\0';
    echoWave received = decodeMessage(buffer);

    {
      std::lock_guard<std::mutex> lock(
          consoleMutex);  // Lock the console output

      std::cout << std::endl << "Received Message:" << std::endl;
      if (received.has_response()) {
        echoWaveResponse response = received.response();
        std::cout << "  Response Status: " << response.status() << std::endl;
        std::cout << "  Response Echo: " << response.echo() << std::endl;

      } else {
        std::cerr << "Unknown Message Type received" << std::endl;
        // Add decode error string
      }

      std::cout << "You: ";  // Prompt for user input
    }
  }
}
