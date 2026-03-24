#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PATTERN "0123456789"
#define PATTERN_LEN 10

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Su dung: %s <PORT>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    printf("Server dang lang nghe...\n");

    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

    char buffer[BUFFER_SIZE];
    char remain[PATTERN_LEN] = {0}; 
    int total_count = 0;

    while (1) {
        int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';

        // Ghép remain + buffer
        char combined[BUFFER_SIZE + PATTERN_LEN];
        strcpy(combined, remain);
        strcat(combined, buffer);

        // Đếm số lần xuất hiện
        char *ptr = combined;
        while ((ptr = strstr(ptr, PATTERN)) != NULL) {
            total_count++;
            ptr += PATTERN_LEN;
        }

        int len = strlen(combined);
        if (len >= PATTERN_LEN - 1) {
            strncpy(remain, combined + len - (PATTERN_LEN - 1), PATTERN_LEN - 1);
            remain[PATTERN_LEN - 1] = '\0';
        }

        printf("So lan xuat hien '%s': %d\n", PATTERN, total_count);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}