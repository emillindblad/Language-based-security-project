#include <asm-generic/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    // Avoids "Address in use" error
    int reuse = -1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEPORT failed: %s \n", strerror(errno));
        return 1;
    }

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

int handle_connection(int client_fd) {
    char buf[1024];
    int b_recived = recv(client_fd, &buf, 1024, NULL);
    char* method = strdup(buf);
    method = strtok(method, " ");
    char* req_path = strtok(NULL, " ");

    printf("Method: %s\n", method);
    printf("Path: %s\n", req_path);

    if (strcmp(method, "GET") != 0 ) {
        // Method not allowed
        char *res = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        send(client_fd, res, strlen(res), 0);
    } else if (strcmp(req_path, "/") == 0 || strcmp(req_path, "/index.html") == 0 ) {
        // Send 200
        char *res = "HTTP/1.1 200 OK\r\n\r\n";
        send(client_fd, res, strlen(res), 0);

    } else {
        //Send 404
        char *res = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_fd, res, strlen(res), 0);
    }

    return 0;
}

int main() {
    int port = 3000;
    int server_fd = setup_server(port);

    printf("server_fd %d\n",server_fd);

    while (1) {
        struct sockaddr_in client_conn;
        socklen_t client_conn_len = sizeof(client_conn);
        int client_fd = accept(
            server_fd,
            (struct sockaddr *)&client_conn,
            &client_conn_len);

        if (client_fd < 0) {
            perror("AAAAAH");
            exit(EXIT_FAILURE);
        } else {
            handle_connection(client_fd);
        }

        close(client_fd);
    }
    return 0;
}

