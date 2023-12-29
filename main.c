#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>

void send_response(int client_fd, const char *content_type, const char *content) {
    char response[1024];
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n%s", content_type, content);
    send(client_fd, response, strlen(response), 0);
}

int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        AF_INET,
        htons(8080), // Use a specific port, e.g., 8080
        0
    };

    bind(s, (struct sockaddr*)&addr, sizeof(addr));

    listen(s, 10);

    int client_fd = accept(s, 0, 0);

    char buffer[1024] = {0};
    recv(client_fd, buffer, sizeof(buffer), 0);

    // Assuming the request is a simple GET request
    if (strncmp(buffer, "GET ", 4) == 0) {
        char* f = buffer + 5;
        char* end = strchr(f, ' ');
        if (end != NULL) {
            *end = 0;
            int opened_fd = open(f, O_RDONLY);
            if (opened_fd != -1) {
                // Determine file size
                off_t file_size = lseek(opened_fd, 0, SEEK_END);
                lseek(opened_fd, 0, SEEK_SET);

                // Send HTTP response
                send_response(client_fd, "text/html", "");

                // Send file content
                sendfile(client_fd, opened_fd, 0, file_size);

                close(opened_fd);
            }
        }
    }

    close(client_fd);
    close(s);

    return 0;
}
