// WinSock2 : API réseau utilisée par Window
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Nécessaire pour l’édition de liens sous Visual Studio

#include <iostream>  // Affichage console
#include <algorithm>
#include <thread>
#include <algorithm>
#include <vector>

#include "server.h"
#include "clientHandler.h"


// Nombre maximum de clients autorisés simultanément
#define MAX_CLIENTS 15


// ---------------------------------------------------------------------------
// Constructeur : initialise les attributs de la classe
// ---------------------------------------------------------------------------
Server::Server(int port)
    : port(port),              // Stocke le port choisi
      serverSocket(INVALID_SOCKET) // Socket non encore créé
{}


// ---------------------------------------------------------------------------
// start() : point d’entrée du serveur TCP
// - Initialise WinSock
// - Crée le socket serveur
// - Configure l’adresse
// - Bind + Listen
// - Lance la boucle d’acceptation
// ---------------------------------------------------------------------------
void Server::start() {

    // 1) Initialisation de WinSock (obligatoire sous Windows)
    WSADATA wsa;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (wsaResult != 0) {
        std::cerr << "Erreur WSAStartup : " << wsaResult << "\n";
        return;
    }

    // 2) Création du socket serveur
    // AF_INET      → IPv4
    // SOCK_STREAM  → TCP (connexion fiable)
    // IPPROTO_TCP  → protocole TCP explicitement
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Erreur socket() : " << WSAGetLastError() << "\n";
        WSACleanup();
        return;
    }

    // 3) Préparation de la structure d’adresse du serveur
    sockaddr_in addr{};
    addr.sin_family = AF_INET;               // IPv4
    addr.sin_port = htons(port);             // Port converti en format réseau
    addr.sin_addr.s_addr = INADDR_ANY;       // Accepte toutes les IP entrantes

    // 4) bind() : associe le socket au port
    if (bind(serverSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Erreur bind() : " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // 5) listen() : met le serveur en mode écoute
    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) {
        std::cerr << "Erreur listen() : " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Serveur en écoute sur le port " << port << "\n";

    // 6) Boucle principale : accepte les clients
    acceptClients();
}


// ---------------------------------------------------------------------------
// acceptClients() : boucle infinie qui accepte les nouveaux clients
// - Chaque client accepté reçoit un thread dédié
// - Si la limite est atteinte → connexion refusée
// ---------------------------------------------------------------------------
void Server::acceptClients() {
    while (true) {

        // Acceptation d’un nouveau client (appel bloquant)
        SOCKET cs = accept(serverSocket, nullptr, nullptr);

        if (cs == INVALID_SOCKET) {
            std::cerr << "Erreur accept() : " << WSAGetLastError() << "\n";
            continue;
        }

        // Protection de la liste des clients (accès multi‑thread)
        std::lock_guard<std::mutex> lock(clientMutex);

        // Vérifie si la limite est atteinte
        if (clients.size() >= MAX_CLIENTS) {
            std::cout << "Connexion refusée : trop de clients.\n";
            closesocket(cs); // On ferme le socket du client refusé
            continue;
        }

        // Ajoute le client à la liste
        clients.push_back(cs);
        std::cout << "Client connecté.\n";

        // Lance un thread pour gérer ce client
        std::thread(ClientHandler(cs, this)).detach();
    }
}


// ---------------------------------------------------------------------------
// broadcast() : envoie un message à tous les clients sauf l’émetteur
// ---------------------------------------------------------------------------
void Server::broadcast(const std::string& msg, SOCKET sender) {

    // Protection de la liste des clients
    std::lock_guard<std::mutex> lock(clientMutex);

    // Parcourt tous les clients connectés
    for (SOCKET c : clients) {

        // On n’envoie pas le message à l’émetteur
        if (c != sender) {
            send(c, msg.c_str(), (int)msg.size(), 0);
        }
    }
}


// ---------------------------------------------------------------------------
// removeClient() : supprime un client de la liste et ferme son socket
// ---------------------------------------------------------------------------
void Server::removeClient(SOCKET cs) {

    // Protection de la liste des clients
    std::lock_guard<std::mutex> lock(clientMutex);

    // Retire le client de la liste
    clients.erase(std::remove(clients.begin(), clients.end(), cs), clients.end());

    // Ferme le socket du client
    closesocket(cs);

    std::cout << "Client déconnecté.\n";
}
