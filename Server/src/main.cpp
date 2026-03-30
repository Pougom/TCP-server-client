#include "server.h"
#include <iostream>

int main() {
    int port = 5000;  // Choisis ton port

    std::cout << "Démarrage du serveur sur le port " << port << "...\n";

    Server server(port);
    // le serveur tourne en boucle infinie en attendant les clients
    server.start();   

    return 0;
}
