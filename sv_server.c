#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 1024

// Struct phải giống client
struct SinhVien {
    char mssv[20];
    char hoten[50];
    char ngaysinh[20];
    float diemTB;
};

int main(int argc, char *argv[]) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    if (argc != 3) {
        printf("Su dung: %s <PORT> <LOG_FILE>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    char *log_file = argv[2];

    // Tạo socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Khong the tao socket");
        return 1;
    }

    // Cấu hình server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind that bai");
        return 1;
    }

    // Listen
    if (listen(server_fd, 5) < 0) {
        perror("Listen that bai");
        return 1;
    }

    printf("Server dang lang nghe cong %d...\n", port);

    while (1) {
        client_len = sizeof(client_addr);

        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept that bai");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        struct SinhVien sv;
        int bytes = recv(client_fd, &sv, sizeof(sv), 0);

        if (bytes > 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);

            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

            printf("\nNhan duoc du lieu tu client:\n");
            printf("IP: %s\n", client_ip);
            printf("Thoi gian: %s\n", time_str);
            printf("MSSV: %s\n", sv.mssv);
            printf("Ho ten: %s\n", sv.hoten);
            printf("Ngay sinh: %s\n", sv.ngaysinh);
            printf("Diem TB: %.2f\n", sv.diemTB);

            FILE *f = fopen(log_file, "a");
            if (f == NULL) {
                perror("Khong mo duoc file log");
            } else {
                fprintf(f, "%s %s %s %s %s %.2f\n",
                        client_ip,
                        time_str,
                        sv.mssv,
                        sv.hoten,
                        sv.ngaysinh,
                        sv.diemTB);
                fclose(f);
            }
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}