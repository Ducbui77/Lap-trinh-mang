#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <ctype.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

typedef struct {
    int fd;
    int step;
    char hoten[100];
    char mssv[30];
} Client;

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void tao_email(char *hoten, char *mssv, char *email) {
    char ho[30], dem[30], ten[30];
    char last6[10];

    strcpy(last6, mssv + strlen(mssv) - 6);

    sscanf(hoten, "%s %s %s", ho, dem, ten);

    sprintf(email, "%s.%c%c%s@sis.hust.edu.vn",
            ten,
            toupper(ho[0]),
            toupper(dem[0]),
            last6);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Su dung: %s <PORT>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    set_nonblocking(server_fd);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 10);

    printf("Server dang chay non-blocking tai cong %d...\n", port);

    Client clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].fd = -1;

    fd_set readfds;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        int maxfd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd != -1) {
                FD_SET(clients[i].fd, &readfds);
                if (clients[i].fd > maxfd) maxfd = clients[i].fd;
            }
        }

        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        // Client mới
        if (FD_ISSET(server_fd, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);

            int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);

            if (client_fd >= 0) {
                set_nonblocking(client_fd);

                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].fd == -1) {
                        clients[i].fd = client_fd;
                        clients[i].step = 0;
                        send(client_fd, "Nhap ho ten: ", 13, 0);
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd == -1) continue;

            int fd = clients[i].fd;

            if (FD_ISSET(fd, &readfds)) {
                char buffer[BUFFER_SIZE];
                int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

                if (bytes <= 0) {
                    close(fd);
                    clients[i].fd = -1;
                    continue;
                }

                buffer[bytes] = '\0';
                buffer[strcspn(buffer, "\r\n")] = '\0';

                if (clients[i].step == 0) {
                    strcpy(clients[i].hoten, buffer);
                    clients[i].step = 1;
                    send(fd, "Nhap MSSV: ", 11, 0);
                } else {
                    strcpy(clients[i].mssv, buffer);

                    char email[200];
                    tao_email(clients[i].hoten, clients[i].mssv, email);

                    strcat(email, "\n");
                    send(fd, email, strlen(email), 0);

                    close(fd);
                    clients[i].fd = -1;
                }
            }
        }
    }

    return 0;
}