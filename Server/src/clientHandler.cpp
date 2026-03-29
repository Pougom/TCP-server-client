#include "clientHandler.h"
#include "server.h"
#include <iostream>
#include <vector>
#include <winsock2.h>

ClientHandler::ClientHandler(SOCKET clientSocket, Server* server)
    : clientSocket(clientSocket), server(server) {}

void ClientHandler::operator()() {

    char buffer[1024];

    while (true) {

        // Réception du message
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            std::cout << "Client déconnecté.\n";
            server->removeClient(clientSocket);
            return;
        }

        // Convertit en string
        std::string msg(buffer, bytesReceived);

        // Diffuse le message aux autres clients
        server->broadcast(msg, clientSocket);
    }
}
