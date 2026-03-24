#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Su dung: %s <PORT>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Khong tao duoc socket");
        return 1;
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind
    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind that bai");
        return 1;
    }

    printf("UDP Echo Server dang chay tren cong %d...\n", port);

    char buffer[BUFFER_SIZE];

    while (1) {
        int bytes = recvfrom(sock, buffer, BUFFER_SIZE, 0,
                             (struct sockaddr*)&client_addr, &client_len);

        if (bytes < 0) {
            perror("Loi recvfrom");
            continue;
        }

        buffer[bytes] = '\0';

        printf("Nhan duoc: %s\n", buffer);

        // Echo lại cho client
        sendto(sock, buffer, bytes, 0,
               (struct sockaddr*)&client_addr, client_len);
    }

    close(sock);
    return 0;
}