#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int c, read_size;
    char client_message[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(serverSocket, 3);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while ((clientSocket = accept(serverSocket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        puts("Connection accepted");

        memset(client_message, 0, BUFFER_SIZE); // Clear the buffer
        read_size = recv(clientSocket, client_message, BUFFER_SIZE, 0);
        if (read_size > 0) {
            // Add one to the received number
            int number = atoi(client_message);
            sprintf(client_message, "%d", number + 1);
            send(clientSocket, client_message, strlen(client_message), 0);
        } else if (read_size == 0) {
            puts("Client disconnected");
        } else {
            perror("recv failed");
        }

        closesocket(clientSocket);
    }

    if (clientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        return 1;
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
