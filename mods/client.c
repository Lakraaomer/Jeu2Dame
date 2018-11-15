#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
 
 
int client(void)
{
    #if defined (WIN32)
        WSADATA WSAData;
        int erreur = WSAStartup(MAKEWORD(2,2), &WSAData);
    #else
        int erreur = 0;
    #endif
 
    SOCKET sock;
    SOCKADDR_IN sin;
    char buffer[32] = "";
 
    /* Si les sockets Windows fonctionnent */
    if(!erreur)
    {
        /* Création de la socket */
        sock = socket(AF_INET, SOCK_STREAM, 0);
 
        /* Configuration de la connexion */
        sin.sin_addr.s_addr = inet_addr("127.0.0.1");
        sin.sin_family = AF_INET;
        sin.sin_port = htons(PORT);
 
        /* Si l'on a réussi à se connecter */
        if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR)
        {
            printf("Connection à %s sur le port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
            
            /* Si l'on reçoit des informations : on les affiche à l'écran */
            if(recv(sock, buffer, 32, 0) != SOCKET_ERROR)
                printf("Recu : %s\n", buffer);
        }
        /* sinon, on affiche "Impossible de se connecter" */
        else
        {
            printf("Impossible de se connecter\n");
            return EXIT_FAILURE;
        }
 
        /* On ferme la socket */
        closesocket(sock);
 
        #if defined (WIN32)
            WSACleanup();
        #endif

    }else{
        
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}