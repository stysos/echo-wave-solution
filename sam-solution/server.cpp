#include "server.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <shared_mutex>
#include <thread>

#include "echoWave.pb.h"

Server::Server(int port) {
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    std::cerr << "Error creating socket.\n";
    std::exit(EXIT_FAILURE);
  }

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(port);

  if (bind(serverSocket, reinterpret_cast<struct sockaddr *>(&serverAddress),
           sizeof(serverAddress)) == -1) {
    std::cerr << "Error binding socket.\n";
    close(serverSocket);
    std::exit(EXIT_FAILURE);
  }

  if (listen(serverSocket, 5) == -1) {
    std::cerr << "Error listening for connections.\n";
    close(serverSocket);
    std::exit(EXIT_FAILURE);
  }

  std::cout << "Server listening on port " << port << std::endl;
}

Server::~Server() { close(serverSocket); }

int Server::getSocket() { return serverSocket; }

void Server::broadcastMessage(int sender, const std::string &message) {
  if (Server::clients.empty()) {
    std::cout << "Clients are empty" << std::endl;
    return;
  }
  std::shared_lock<std::shared_timed_mutex> lock(clientsMutex);

  for (int clientSocket : Server::clients) {
    if (clientSocket == sender) {
      std::cout << "Sending echo to " << clientSocket << std::endl;
      send(clientSocket, message.c_str(), message.size(), 0);
    }
  }
}

void Server::acceptConnections() {
  while (true) {
    // TODO: Add client socket to vector - keep open and poll if still open]
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1) {
      std::cerr << "Error accepting connection.\n";
      close(serverSocket);
      std::exit(EXIT_FAILURE);
    }

    Server::clients.push_back(clientSocket);

    std::thread(&Server::handleClient, this, clientSocket).detach();
  }
}

echoWave Server::decodeMessage(char *message) {
  echoWave msg{};
  if (msg.ParseFromString(message)) {
    if (msg.has_request()) {
      echoWaveRequest request = msg.request();
      return msg;
    } else if (msg.has_response()) {
      std::cerr << "Received response from client" << std::endl;
      // Add decode error restring
      return msg;
    } else {
      std::cerr << "Unknown Message Type received" << std::endl;
      // Add decode error string
      return msg;
    }
  }
  // Add decode error string
  return msg;
}

std::string Server::buildResponse(const echoWaveRequest &request) {
  echoWave responseMsg{};
  // std::cout << "Building response " << std::endl;

  echoWaveResponse *response = responseMsg.mutable_response();

  response->set_status(true);
  response->set_echo(request.wave());

  std::string responseStr;

  if (responseMsg.IsInitialized()) {
    if (responseMsg.SerializeToString(&responseStr)) {
      // std::cout << "Returning response str " << responseStr << std::endl;
      return responseStr;
    } else {
      std::cerr << "Failed to serialize response message.\n";
    }
  } else {
    std::cerr << "Error: Response message is not properly initialized.\n";
  }

  response->set_status(false);
  response->set_echo("Error building message");
  responseMsg.SerializeToString(&responseStr);
  return responseStr;
}

std::string Server::buildError(std::string errorString) {
  std::cout << "Building error " << std::endl;
  echoWave responseMsg{};

  echoWaveResponse *response = responseMsg.mutable_response();

  response->set_status(false);
  response->set_echo(errorString);

  std::string responseStr;
  responseMsg.SerializeToString(&responseStr);

  return responseStr;
}

void Server::handleClient(int clientSocket) {
  char buffer[1024];
  std::string response;

  while (true) {
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
      std::cerr << "Connection closed by client.\n";
      close(clientSocket);

      clients.erase(std::remove(clients.begin(), clients.end(), clientSocket),
                    clients.end());

      break;
    }

    buffer[bytesRead] = '\0';
    echoWave received = decodeMessage(buffer);

    if (received.has_request()) {
      echoWaveRequest request = received.request();
      response = buildResponse(request);
    } else {
      response = buildError("Error: Unexpected message type received");
    }

    std::cout << "Client " << clientSocket << ": " << received.DebugString()
              << std::endl;

    std::cout << "Serialised response: " << response << std::endl;

    std::thread(&Server::broadcastMessage, this, clientSocket, response)
        .detach();
  }
}
