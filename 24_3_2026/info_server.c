#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096

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

    char buffer[BUFFER_SIZE] = {0};
    recv(client_fd, buffer, BUFFER_SIZE, 0);

    printf("\n=== Thong tin nhan duoc ===\n");

    char *line = strtok(buffer, "\n");

    // Dòng đầu: thư mục
    printf("%s\n", line);

    // Các dòng sau: file
    while ((line = strtok(NULL, "\n")) != NULL) {
        char *name = strtok(line, "|");
        char *size = strtok(NULL, "|");

        if (name && size) {
            printf("%s - %s bytes\n", name, size);
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}