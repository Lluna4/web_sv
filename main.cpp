#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <string>
#include <filesystem>
#include <iostream>
#include <chrono>
#include "logging.hpp"
#include "config.hpp"

int PORT = 8082;

void send_response(int new_s)
{
    char response[2048];
    std::string path;
    char buffer[2048] = {0};
    char ins[2048] = {0};
    int index = 0;
    int fd;
    off_t file_size;

    recv(new_s, buffer, 2048, 0);
    if (*buffer == '\0')
        return;
    memccpy(ins, buffer, ' ', 4);
    if (strcmp(ins, "GET ") == 0) 
    {
        char *f = buffer + 5;
        *strchr(f, ' ') = 0;
        path = f;
        if (*f != '\0' && std::filesystem::exists(path) == false) 
        {
            log(f);
            sprintf(response, "HTTP/1.0 404 Not found\r\nConnection: close\r\nServer: Luna\r\n\r\n");
            log("Responded: ", "HTTP/1.0 404 Not found");
            send(new_s, response, strlen(response), 0);
            close(new_s);
            return;
        }
        if (*f == '\0')
        {
            fd = open("index.html", O_RDONLY);
            file_size = lseek(fd, 0, SEEK_END);
        }
        else
        {
            fd = open(f, O_RDONLY);
            file_size = lseek(fd, 0, SEEK_END);
        }
        lseek(fd, 0, SEEK_SET);
        sprintf(response, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\nConnection: close\r\nServer: Luna\r\n\r\n%s", "text/html", "");
        send(new_s, response, strlen(response), 0);
        sendfile(new_s, fd, 0, file_size);
        close(fd);
    }
    log("Responded: ", "HTTP/1.0 200 OK");
    close(new_s); 
}

int main() 
{
    log("Starting server...");
    int s = socket(AF_INET, SOCK_STREAM, 0);
    log("Loading config...");
    if (std::filesystem::exists("http.cfg") == false)
	    create_config();
    load_config(&PORT);
    struct sockaddr_in addr = {
        AF_INET,
        htons(PORT),
        0
    };

    if (bind(s, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        log("Bind failed");
        return -1;
    }
    log("Listening on port ", PORT);
    log("Website available at http:/localhost:", PORT, "/");
    while (1) 
    {
        listen(s, 10);
        int new_s = accept(s, 0, 0);
        std::thread response_th(send_response, new_s);
        response_th.detach();
    }
    close(s);
    return 0;
}

