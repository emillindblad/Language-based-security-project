#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/resource.h>
#include <semaphore.h>

sem_t mutex;

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
        perror("SO_REUSEPORT failed");
        return 1;
    }

    socket_conf.sin_family = AF_INET;
    socket_conf.sin_port = htons(port);
    socket_conf.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr*)&socket_conf, (socklen_t)sizeof(socket_conf)) < 0) {
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

void create_ok_response(int client_fd) {
    FILE* fptr = fopen("../html/index.html", "r");

    fseek(fptr, 0, SEEK_END);
    size_t file_size = ftell(fptr);
    rewind(fptr);

    char* body;
    body = (char*)malloc(file_size);
    fread(body, 1, file_size, fptr);
    fclose(fptr);

    char res[1024];
    sprintf(res,
            "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s",
            file_size,
            body);
    send(client_fd, res, strlen(res), 0);
    free(body);
}


void* handle_connection(void* arg) {
    //Type cast the generic pointer to an int pointer and dereference the address.
    int client_fd = *((int*)arg);

    char req_buf[1024];
    int buf_written = recv(client_fd, &req_buf, 1024, 0);
    if (buf_written < 1) {
        // printf("Empty request. Closing.\n");
        free(arg);
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
        return 0;
    }

    char* method = strdup(req_buf);
    method = strtok(method, " ");
    char* req_path = strtok(NULL, " ");

    // printf("Method: %s\n", method);
    // printf("Path: %s\n", req_path);

    if (strcmp(method, "GET") != 0 ) {
        // Method not allowed
        char* res = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        send(client_fd, res, strlen(res), 0);

    } else if (strcmp(req_path, "/") == 0 || strcmp(req_path, "/index.html") == 0 ) {
        sem_wait(&mutex);
        create_ok_response(client_fd);
        sem_post(&mutex);
    } else if (strcmp(req_path, "/sleep")) {
        // Sleep for testing concurrency.
        sleep(3);
        char* res = "HTTP/1.1 200 OK\r\n\r\n";
        send(client_fd, res, strlen(res), 0);
    } else {
        char* res = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_fd, res, strlen(res), 0);
    }

    free(arg);
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
    return 0;
}

int main() {
    int port = 3000;
    int server_fd = setup_server(port);

    // Initialize the mutex from 1.
    sem_init(&mutex, 0, 1);

    while (1) {
        struct sockaddr_in client_conn;
        socklen_t client_conn_len = sizeof(client_conn);
        int client_fd = accept(
            server_fd,
            (struct sockaddr*)&client_conn,
            &client_conn_len);
        if (client_fd < 0) {
            perror("Failed to accept connection");
            exit(EXIT_FAILURE);
        } else {
            int* client_fd_ptr = (int*)malloc(sizeof(int));
            pthread_t thread_id;
            if (client_fd_ptr == NULL) {

                perror("Malloc error");
                exit(EXIT_FAILURE);
            }

            *client_fd_ptr = client_fd;
            pthread_create(&thread_id, NULL, handle_connection, client_fd_ptr);
            pthread_detach(thread_id);
        }
    }
    return 0;
}
