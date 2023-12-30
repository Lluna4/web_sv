#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>

void send_response(int new_s)
{
    char response[1024];
    char buffer[256] = {0};
    recv(new_s, buffer, 256, 0);
    if (*buffer == '\0')
        return ;
    char *f = buffer + 5;
    *strchr(f, ' ') = 0;
    int fd = open(f, O_RDONLY);
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n%s", "text/html", "");
    send(new_s, response, strlen(response), 0);
    sendfile(new_s, fd, 0, file_size);
    close(new_s);
    close(fd);
}

int main()
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        AF_INET,
        htons(8082),
        0
    };
    bind(s, (sockaddr *)&addr, sizeof(addr));
    while (1)
    {
        listen(s, 10);
        int new_s = accept(s, 0, 0);
        std::thread response_th(send_response, new_s);
        response_th.detach();
    }
    close(s);
}