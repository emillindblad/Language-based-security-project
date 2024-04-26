#include <netinet/in.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"

#define PORT 8080

int server_connect(int port) {
    int server_fd;

    struct sockaddr_in server_address;
    socklen_t addr_len = (socklen_t)sizeof(server_address);

    // create new server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(0);
    }

    // config socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // bind to socket
    if (bind(server_fd, (struct sockaddr*)&server_address, addr_len) < 0) {
        perror("bind failed");
        exit(0);
    }

    // listen to incoming connections
    if (listen(server_fd, 10) < 0) {   // listen to max 10 connections
        perror("listen failed");
        exit(0);
    }

    printf("Server listening to port: %i \n", PORT);
    return server_fd;
}

int main(int argc, char *argv[]) {
    int port = PORT;
    int server_fd = server_connect(port);

    while(1) {
        
        //client info
        struct sockaddr_in client_address;
        socklen_t client_addr_len = sizeof(client_address);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addr_len);

        // accept client connection
        if ((client_fd < 0)) {
            perror("accept failed");
            exit(0);
        }
        
        printf("connection established. client: %d, port: %i\n", client_fd, PORT);

        char buf[256];
        if (read(client_fd, &buf, sizeof(buf))) {
            perror("connect failed\n");
            exit(0);
        }
        close(client_fd);
        // create new thread to handle new client 
        //pthread_t thread_id;
        //pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
        
    }
    return 0;
}