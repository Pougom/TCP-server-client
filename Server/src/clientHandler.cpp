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
        // recv() lit les données envoyées par le client.
        // buffer reçoit les données brutes.
        // bytesReceived indique combien d’octets ont été reçus.
        // Si le client envoie "Salut", bytesReceived = 5
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        // Si recv() retourne 0, le client a fermé proprement sa connexion. Si -1 → erreur réseau ou déconnexion brutale
        // Dans les deux cas, le serveur : affiche un message, retire le client de la liste, ferme son socket, quitte le thread (return)
        if (bytesReceived <= 0) {
            std::cout << "Client déconnecté.\n";
            server->removeClient(clientSocket);
            return;
        }

        // Convertit en string
        std::string msg(buffer, bytesReceived);

        // Diffuse le message aux autres clients
        // Le serveur envoie le message à tous les autres clients, sauf l’émetteur.
        server->broadcast(msg, clientSocket);
    }
}
