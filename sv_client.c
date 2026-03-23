#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

struct SinhVien {
    char mssv[20];
    char hoten[50];
    char ngaysinh[20];
    float diemTB;
};

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    struct SinhVien sv;

    if (argc != 3) {
        printf("Su dung: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // Tạo socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Khong the tao socket");
        return 1;
    }

    // Cấu hình server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("Dia chi IP khong hop le\n");
        return 1;
    }

    // Kết nối server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ket noi that bai");
        return 1;
    }

    printf("Da ket noi den server %s:%d\n", server_ip, port);

    printf("Nhap MSSV: ");
    fgets(sv.mssv, sizeof(sv.mssv), stdin);
    sv.mssv[strcspn(sv.mssv, "\n")] = 0;

    printf("Nhap ho ten: ");
    fgets(sv.hoten, sizeof(sv.hoten), stdin);
    sv.hoten[strcspn(sv.hoten, "\n")] = 0;

    printf("Nhap ngay sinh (dd/mm/yyyy): ");
    fgets(sv.ngaysinh, sizeof(sv.ngaysinh), stdin);
    sv.ngaysinh[strcspn(sv.ngaysinh, "\n")] = 0;

    printf("Nhap diem trung binh: ");
    scanf("%f", &sv.diemTB);

    send(sock, &sv, sizeof(sv), 0);

    printf("Da gui thong tin sinh vien den server!\n");

    close(sock);
    printf("Dong ket noi.\n");

    return 0;
}