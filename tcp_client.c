#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Kiểm tra tham số dòng lệnh
    if (argc != 3) {
        printf("Sử dụng: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // Tạo socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Tạo socket thất bại");
        return 1;
    }

    // Cấu hình địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Địa chỉ không hợp lệ");
        return 1;
    }

    // Kết nối tới server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Kết nối thất bại");
        return 1;
    }

    printf("Đã kết nối tới server %s:%d\n", server_ip, port);

    // Gửi dữ liệu từ bàn phím
    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Thoát nếu nhập "exit"
        if (strncmp(buffer, "exit", 4) == 0)
            break;

        send(sock, buffer, strlen(buffer), 0);
    }

    // Đóng socket
    close(sock);
    printf("Disconnected.\n");

    return 0;
}