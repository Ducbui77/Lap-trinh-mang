#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Su dung: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    char buffer[BUFFER_SIZE];

    while (1) {
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';
        printf("%s", buffer);

        fgets(buffer, sizeof(buffer), stdin);
        send(sock, buffer, strlen(buffer), 0);
    }

    close(sock);
    return 0;
}