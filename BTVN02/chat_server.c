#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/select.h>

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct
{
    int fd;
    char client_id[50];
    char client_name[50];
    int registered;
} Client;

void init_clients(Client clients[])
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].fd = -1;
        clients[i].registered = 0;
        memset(clients[i].client_id, 0, sizeof(clients[i].client_id));
        memset(clients[i].client_name, 0, sizeof(clients[i].client_name));
    }
}

void get_timestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y/%m/%d %I:%M:%S%p", t);
}

void broadcast_message(Client clients[], int sender_idx, const char *message)
{
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    char final_msg[BUFFER_SIZE + 200];
    snprintf(final_msg, sizeof(final_msg),
             "%s %s: %s\n",
             timestamp,
             clients[sender_idx].client_id,
             message);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].fd != -1 &&
            clients[i].registered &&
            i != sender_idx)
        {
            send(clients[i].fd, final_msg, strlen(final_msg), 0);
        }
    }

    printf("%s", final_msg);
}

int add_new_client(Client clients[], int new_fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].fd == -1)
        {
            clients[i].fd = new_fd;
            clients[i].registered = 0;
            return i;
        }
    }
    return -1;
}

int find_client_index(Client clients[], int fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].fd == fd)
            return i;
    }
    return -1;
}

void remove_client(Client clients[], int idx)
{
    close(clients[idx].fd);
    clients[idx].fd = -1;
    clients[idx].registered = 0;
}

int main()
{
    int listen_fd, new_fd, max_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    fd_set master_set, read_fds;
    Client clients[MAX_CLIENTS];

    init_clients(clients);

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listen_fd, 5);

    FD_ZERO(&master_set);
    FD_SET(listen_fd, &master_set);
    max_fd = listen_fd;

    printf("Chat server dang chay tai cong %d...\n", PORT);

    while (1)
    {
        read_fds = master_set;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0)
        {
            perror("select error");
            exit(1);
        }

        for (int fd = 0; fd <= max_fd; fd++)
        {
            if (!FD_ISSET(fd, &read_fds))
                continue;

            if (fd == listen_fd)
            {
                // Client mới kết nối
                new_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);

                int idx = add_new_client(clients, new_fd);

                if (idx == -1)
                {
                    close(new_fd);
                    continue;
                }

                FD_SET(new_fd, &master_set);
                if (new_fd > max_fd)
                    max_fd = new_fd;

                char *welcome =
                    "Nhap theo cu phap: client_id: client_name\n";
                send(new_fd, welcome, strlen(welcome), 0);
            }
            else
            {
                char buf[BUFFER_SIZE];
                int nbytes = recv(fd, buf, sizeof(buf) - 1, 0);

                int idx = find_client_index(clients, fd);

                if (nbytes <= 0)
                {
                    FD_CLR(fd, &master_set);
                    remove_client(clients, idx);
                    continue;
                }

                buf[nbytes] = '\0';
                buf[strcspn(buf, "\r\n")] = 0;

                if (!clients[idx].registered)
                {
                    char id[50], name[50];

                    if (sscanf(buf, "%49[^:]: %49s", id, name) == 2)
                    {
                        strcpy(clients[idx].client_id, id);
                        strcpy(clients[idx].client_name, name);
                        clients[idx].registered = 1;

                        char *ok = "Dang ky thanh cong! Co the chat.\n";
                        send(fd, ok, strlen(ok), 0);
                    }
                    else
                    {
                        char *retry =
                            "Sai cu phap. Dung: abc: NguyenVanA\n";
                        send(fd, retry, strlen(retry), 0);
                    }
                }
                else
                {
                    broadcast_message(clients, idx, buf);
                }
            }
        }
    }

    return 0;
}