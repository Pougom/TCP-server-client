#ifndef SERVER_H
#define SERVER_H

// WinSock2 doit être inclus AVANT <windows.h> et avant tout autre include réseau
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <algorithm>
#include <vector>   // Stockage des sockets clients
#include <mutex>    // Protection multi-thread
#include <string>   // std::string

// La classe Server représente un serveur TCP sous Windows
// utilisant WinSock2 pour gérer les connexions réseau.
class Server {
public:
    // Constructeur : initialise le serveur avec un numéro de port.
    Server(int port);

    // Démarre le serveur : initialisation WinSock, création du socket,
    // bind, listen, puis boucle d'acceptation.
    void start();

    // Envoie un message à tous les clients sauf celui qui l'a envoyé.
    void broadcast(const std::string& msg, SOCKET senderSocket);

    // Supprime un client de la liste lorsqu'il se déconnecte.
    void removeClient(SOCKET clientSocket);

private:
    SOCKET serverSocket;      // Socket principal du serveur (type Windows)
    int port;                 // Port d'écoute du serveur

    // Liste des sockets clients connectés
    std::vector<SOCKET> clients;

    // Mutex pour protéger l'accès à la liste des clients
    std::mutex clientMutex;

    // Boucle interne : accepte les nouveaux clients en continu.
    void acceptClients();
};

#endif
