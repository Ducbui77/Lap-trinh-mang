#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Su dung: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Khong tao duoc socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ket noi that bai");
        return 1;
    }

    printf("Da ket noi server!\n");

    char buffer[BUFFER_SIZE] = {0};

    char cwd[256];
    getcwd(cwd, sizeof(cwd));

    strcat(buffer, cwd);
    strcat(buffer, "\n");

    DIR *dir = opendir(".");
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        struct stat st;

        if (stat(entry->d_name, &st) == 0 && S_ISREG(st.st_mode)) {
            char line[512];
            snprintf(line, sizeof(line), "%s|%ld\n", entry->d_name, st.st_size);
            int len = strlen(buffer);
            snprintf(buffer + len, BUFFER_SIZE - len, "%s|%ld\n", entry->d_name, st.st_size);
        }
    }

    closedir(dir);

    send(sock, buffer, strlen(buffer), 0);

    printf("Da gui thong tin thu muc!\n");

    close(sock);
    return 0;
}