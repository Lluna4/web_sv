#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #include <mswsock.h>
    #pragma comment(lib, "Ws2_32.lib")
    #define WINDOWS
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/sendfile.h>
#endif
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
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
    log(buffer);
    if (*buffer == '\0')
        return;
    memccpy(ins, buffer, ' ', 4);
    if (strcmp(ins, "GET ") == 0) 
    {
        char *f = buffer + 5;
        *strchr(f, ' ') = 0;
        path = f;
        if (*f != '\0' && std::filesystem::exists(path) == false && (urls.empty() || !urls.contains(f))) 
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
            if (!urls.empty() && strstr(f, ".html") == nullptr)
            {
                if (urls.contains(f))
                {
                    f = strdup(urls[f].get_direction().c_str());
                }
            }
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
    #ifdef WINDOWS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }
    SOCKET s;
    #else
    int s = 0;
    #endif
    log("Starting server...");
    s = socket(AF_INET, SOCK_STREAM, 0);
    log("Loading config...");
    if (std::filesystem::exists("http.cfg") == false)
	    create_config();
    if (std::filesystem::exists("urls.cfg"))
        load_url_config();
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
        #ifdef WINDOWS
        SOCKET new_s = accept(s, 0, 0);
        #else
        int new_s = accept(s, 0, 0);
        #endif
        std::thread response_th(send_response, new_s);
        response_th.detach();
    }
    close(s);
    return 0;
}

