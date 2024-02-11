#include <signal.h>

#include <iostream>
#include <memory>

#include "client.hpp"
#include "server.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Please enter a port " << argv[0] << " <port>" << std::endl;
    return EXIT_FAILURE;
  }

  int port = std::stoi(argv[1]);

  std::cout << "Choose (1) Server or (2) Client: ";
  int choice;
  std::cin >> choice;

  if (choice == 1) {
    auto server = std::make_unique<Server>(port);
    server->acceptConnections();
  } else if (choice == 2) {
    auto client = std::make_unique<Client>(port);
    client->startChat();
  } else {
    std::cout << "Invalid choice.\n";
    return EXIT_FAILURE;
  }

  return 0;
}