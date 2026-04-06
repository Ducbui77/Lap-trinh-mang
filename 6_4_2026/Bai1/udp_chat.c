#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Su dung: %s <port_s> <ip_d> <port_d>\n", argv[0]);
        return 1;
    }

    int local_port = atoi(argv[1]);
    char *dest_ip = argv[2];
    int dest_port = atoi(argv[3]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Khong tao duoc socket");
        return 1;
    }

    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(local_port);

    if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("Bind that bai");
        return 1;
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dest_port);
    inet_pton(AF_INET, dest_ip, &dest_addr.sin_addr);

    printf("UDP Chat dang chay tai cong %d -> %s:%d\n",
           local_port, dest_ip, dest_port);

    fd_set readfds;
    char buffer[BUFFER_SIZE];

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        int maxfd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;

        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            fgets(buffer, BUFFER_SIZE, stdin);

            sendto(sock, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        }

        if (FD_ISSET(sock, &readfds)) {
            struct sockaddr_in sender_addr;
            socklen_t sender_len = sizeof(sender_addr);

            int bytes = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                 (struct sockaddr*)&sender_addr, &sender_len);

            if (bytes > 0) {
                buffer[bytes] = '\0';
                printf("Tin nhan: %s", buffer);
            }
        }
    }

    close(sock);
    return 0;
}