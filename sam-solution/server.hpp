#pragma once
#include <arpa/inet.h>

#include <csignal>
#include <shared_mutex>
#include <string>
#include <vector>

#include "echoWave.pb.h"

class Server {
 public:
  Server(int port);
  ~Server();

  void acceptConnections();
  int getSocket();

 private:
  int serverSocket;
  sockaddr_in serverAddress;
  std::vector<int> clients;

  mutable std::shared_timed_mutex clientsMutex;  // Shared timed mutex

  void handleClient(int clientSocket);
  void broadcastMessage(int sender, const std::string &message);
  echoWave decodeMessage(char *message);
  std::string buildResponse(const echoWaveRequest &message);
  std::string buildError(std::string errorString);
};