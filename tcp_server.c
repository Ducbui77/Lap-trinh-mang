#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    if (argc != 4) {
        printf("Su dung: %s <PORT> <WELCOME_FILE> <OUTPUT_FILE>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    char *welcome_file = argv[2];
    char *output_file = argv[3];

    // Mở file chứa câu chào
    FILE *wf = fopen(welcome_file, "r");
    if (!wf) {
        perror("Khong thể mở file ");
        return 1;
    }

    char welcome_msg[BUFFER_SIZE];
    fgets(welcome_msg, BUFFER_SIZE, wf);
    fclose(wf);

    // Tạo socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Tạo socket thất bại");
        return 1;
    }

    // Cấu hình địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // lắng nghe mọi IP
    server_addr.sin_port = htons(port);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen
    if (listen(server_fd, 5) < 0) {
        perror("Lang nghe thất bại");
        return 1;
    }

    printf("Server dang nghe tai cong %d...\n", port);

    // Lặp chờ client
    while (1) {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected!\n");

        // Gửi câu chào
        send(client_fd, welcome_msg, strlen(welcome_msg), 0);

        // Mở file ghi dữ liệu
        FILE *of = fopen(output_file, "a");
        if (!of) {
            perror("Cannot open output file");
            close(client_fd);
            continue;
        }

        int bytes;
        while ((bytes = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0) {
            fwrite(buffer, 1, bytes, of);
        }

        fclose(of);
        close(client_fd);
        printf("Client disconnected.\n");
    }

    close(server_fd);
    return 0;
}