#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int server_fd;
    int client_fd;
    struct sockaddr_in address;
    char buffer[4096];

    char *response =
        "HTTP/1.1 200 OK\r\n"


        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>Hello from C HTTP server lol boii</h1>";

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket");
        return 1; }

    // Clear address struct 
    memset(&address,0, sizeof(address));

    // Fill address info
    address.sin_family =AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Bind socket to port 8080  
	//
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    //Start listening
    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Server running at http://localhost:8080\n");

    //Accept one browser/client connection
    client_fd = accept(server_fd, NULL, NULL);

    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        return 1;
    }

    // Read browser request
    int n = read(client_fd, buffer, sizeof(buffer) - 1);

    if (n ==-1) {
        perror("read");
        close(client_fd);
        close(server_fd);
        return 1;
    }

    buffer[n] = '\0';

    printf("Request from browser:\n%s\n", buffer);

    // Send response to browser
    write(client_fd, response, strlen(response));

     //Close client and server socket
    close(client_fd);
    close(server_fd);

    return 0;
}
