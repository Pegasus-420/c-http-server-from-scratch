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
    int opt = 1;

    const char *home_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>Home Page</h1>"
        "<p>This is a tiny C HTTP server.</p>";

    const char *about_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>About</h1>"
        "<p>learning file descriptors , sockets funda in  C.</p>";

    const char *health_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "OK\n";

    const char *not_found_response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<h1>404 Not Found</h1>"
        "<p>This route does not exist buddy.</p>";
       
   	// Create a TCP socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    // Allow reusing port 8080 after restarting the server.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }
    memset(&address, 0, sizeof(address));

    //      Set server address details.
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    //Attach the socket to port 8080.
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }
    //start listening for browser/client connections
    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Server running at http://localhost:8080\n");

    // Keep the server running and handle one client at a time.
    while (1) {
        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        int n = read(client_fd, buffer, sizeof(buffer) - 1);

        if (n == -1) {
            perror("read");
            close(client_fd);
            continue;
        }

        buffer[n] = '\0';

        printf("Request from browser:\n%s\n", buffer);
        const char *response = not_found_response;

        if (strncmp(buffer, "GET / ", strlen("GET / ")) == 0) {
            response = home_response;
        } else if (strncmp(buffer, "GET /about ", strlen("GET /about ")) == 0) {
            response = about_response;
        } else if (strncmp(buffer, "GET /health ", strlen("GET /health ")) == 0) {
            response = health_response;
        }

        if (write(client_fd, response, strlen(response)) == -1) {
            perror("write");
        }
        close(client_fd);

        // Clear buffer before next request.
        memset(buffer, 0, sizeof(buffer));
    }

    close(server_fd);

    return 0;
}
