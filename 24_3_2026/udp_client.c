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

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Khong tao duoc socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    char buffer[BUFFER_SIZE];

    while (1) {
        printf("Nhap du lieu: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Gửi dữ liệu
        sendto(sock, buffer, strlen(buffer), 0,
               (struct sockaddr*)&server_addr, server_len);

        // Nhận echo
        int bytes = recvfrom(sock, buffer, BUFFER_SIZE, 0, NULL, NULL);

        if (bytes > 0) {
            buffer[bytes] = '\0';
            printf("Server tra ve: %s\n", buffer);
        }
    }

    close(sock);
    return 0;
}