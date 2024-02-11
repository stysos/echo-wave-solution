#pragma once

#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <mutex>
#include <thread>

#include "echoWave.pb.h"

class Client {
 public:
  Client(int port);
  ~Client() = default;

  void startChat();
  std::string createMessage(std::string waveString);

 private:
  int clientSocket;
  sockaddr_in serverAddress;

  std::mutex consoleMutex;

  void sendMessage();

  void receiveMessages();
  echoWave decodeMessage(char* message);
};
