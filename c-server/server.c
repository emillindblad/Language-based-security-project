#include <asm-generic/socket.h>
//#include <cstddef>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
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

    // Avoids "Address in use" error
    int reuse = -1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEPORT failed: %s \n", strerror(errno));
        return 1;
    }

    // config socket
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // bind to socket
    if (bind(server_fd, (struct sockaddr *)&server_address, addr_len) < 0) {
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

void create_http_response(int client_fd, char* path) {
    //char *filepath = strdup(path);
   
    FILE *fp = fopen(path, "r");    // open file with only read permissions
    int bytes_sent;
    if (!fp) {
        char *res = "HTTP/1.1 404 Not Found\r\n\r\n"; // HTTP response
        printf("Sending response: %s", res);
        bytes_sent = send(client_fd, res, strlen(res), 0);
        printf("bytes sent:%i\n", bytes_sent);
    } else {
        printf("Open file: %s\n", path);
    }

    // traverse the file?
    if (fseek(fp, 0, SEEK_END) < 0 ) {
        printf("Error reading the document\n");
    }

    // find end of file and save value
    size_t file_size = ftell(fp);

    // rewind cursor to beginning of file
    rewind(fp);

    void* data = malloc(file_size);

    int bytes_read = fread(data, 1, file_size, fp); // fill in the content in the memory location starting at data
    if (bytes_read != file_size) {
        perror("Reading failed\n");
        exit(0);
    }

    fclose(fp);

    char response[1024];
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %ld\r\n\r\n%s", file_size, (char *)data);
    printf("Sending response: %s\n", response);
    bytes_sent = send(client_fd, response, strlen(response), 0);

}

void handle_connection(int client_fd) {
    char buf[256];
    int bytes_recv = read(client_fd, &buf, sizeof(buf));

    if (bytes_recv < 0) {
        perror("connect failed\n");
        exit(0);
    }

    char *method = strdup(buf);
    method = strtok(method, " "); // GET POST PATCH and so on

    char *path = strtok(NULL, " ");

    printf("method: %s\n", method);
    printf("path: %s\n", path);
    
    int bytes_sent; 
    if (strcmp(path, "/test.txt") == 0) {    // change to == 

        create_http_response(client_fd, "./test.txt");
		char *res = "HTTP/1.1 200 OK\r\n\r\n"; // HTTP response
		printf("Sending response: %s\n", res);
		bytes_sent = send(client_fd, res, strlen(res), 0); 

        if (bytes_sent < 0) {
            perror("send failed\n");
            exit(0);
        }
	} 

    printf("buffer: %s \n", buf);
}

int main(int argc, char *argv[]) {
    int port = PORT;
    int server_fd = server_connect(port);

    while(1) {
        
        //client info
        struct sockaddr_in client_address;
        socklen_t client_addr_len = sizeof(client_address);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_addr_len);

        // accept client connection
        if ((client_fd < 0)) {
            perror("accept failed");
            exit(0);
        } else {
            handle_connection(client_fd);
        }
        
        printf("connection established. client: %d, port: %i\n", client_fd, PORT);

       
        close(client_fd);
        // create new thread to handle new client 
        //pthread_t thread_id;
        //pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
        
    }
    return 0;
}