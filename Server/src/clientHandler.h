#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <winsock2.h>
#include <string>

class Server;

// Gère un client dans un thread séparé
class ClientHandler {
public:
    ClientHandler(SOCKET clientSocket, Server* server);

    // Fonction exécutée dans un thread
    void operator()();

private:
    SOCKET clientSocket;   // Socket du client
    Server* server;        // Référence vers le serveur
};

#endif
