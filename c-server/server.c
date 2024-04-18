#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "unistd.h"

int setup_server(int port) {
    int socket_fd;
    struct sockaddr_in socket_conf;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    socket_conf.sin_family = AF_INET;
    socket_conf.sin_port = htons(port);
    socket_conf.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *)&socket_conf, (socklen_t)sizeof(socket_conf)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Bind success\n");

    if (listen(socket_fd, 10) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listen success\n");
    printf("Server running on %d\n",port);

    return socket_fd;
}

int main() {
    int port = 3000;
    int server_fd = setup_server(port);

    printf("server_fd %d\n",server_fd);

    while (1) {
        char buf[256];
        struct sockaddr_in client_conn;
        socklen_t client_conn_len = sizeof(client_conn);
        int client_fd = accept(
            server_fd,
            (struct sockaddr *)&client_conn,
            &client_conn_len);

        if (client_fd < 0) {
            perror("AAAAAH");
            exit(EXIT_FAILURE);
        }

        int valread = read(client_fd, &buf, 256-1);
        printf("Buf: %s\n", buf);
        close(client_fd);
    }
    return 0;
}

