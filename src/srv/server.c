#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include "srvpoll.h"
#include <unistd.h>
#include <string.h>

clientstate_t clientStates[MAX_CLIENTS] = {0};

void handle_client(clientstate_t *client)
{
    char buf[5000] = "Hello client!";
    printf("Buffer %s", client->fd);
    

}

int main()
{

    struct sockaddr_in server_addr, client_addr = {0};
    // struct sockaddr_in clientInfo = {0};
    // int clientSize = 0;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = 0;
    server_addr.sin_port = htons(PORT);
    // unsigned int clientSize = 0;
    socklen_t clientSize = sizeof(client_addr);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("Socket");
        return -1;
    }

    // bind

    if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(fd);
        return -1;
    }

    // listen
    init_clients(&clientStates);

    if (listen(fd, 0) == -1)
    {
        perror("Listen");
        close(fd);
        return -1;
    }

    printf("Server listening on port %d\n", PORT);

    int opt = 1;

    struct pollfd fds[MAX_CLIENTS];

    // int cfd = -1;

    fds[0].fd = fd;
    fds[0].events = POLLIN;
    // fds[1].fd = -1; //init client fd
    // loop to init all fds until MAX_CLIENTS
    for (int i = 1; i < MAX_CLIENTS; i++)
    {
        fds[i].fd = -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
    {
        perror("setsockopt");
    }
    int cfd = 0;
    int freeSlot;
    int nClientFds = 1;
    while (1)
    {
        int ii = 1;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clientStates[i].fd != -1)
            {
                fds[ii].fd = clientStates[i].fd;
                fds[ii].events = POLLIN;
                ii++;
            }
        }

        int n_events = poll(fds, MAX_CLIENTS, -1); // her polles alle FD laget i structen
        // On success, poll() returns a nonnegative
        // value which is the number of elements in the pollfds whose revents fields have been set to a
        // nonzero value

        if (n_events > 0)
        {
            if (fds[0].revents & POLLIN)
            {
                if ((cfd = accept(fd, (struct sockaddr *)&client_addr, &clientSize)) == -1)
                {
                    perror("accept");
                    continue;
                }
                printf("New connection from %s:%d\n",
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                freeSlot = find_free_slot(&clientStates);
                if (freeSlot == -1)
                {
                    printf("Server full: closing new connection\n");
                    close(cfd);
                }
                else
                {
                    clientStates[freeSlot].fd = cfd;
                    clientStates[freeSlot].state = STATE_CONNECTED;
                    nClientFds++;
                    printf("Slot %d has fd %d\n", freeSlot, clientStates[freeSlot].fd);
                }

                n_events--;
            }

            for (int i = 1; i <= nClientFds && n_events > 0; i++)
            {
                if (fds[i].revents & POLLIN)
                {
                    n_events--;

                    int fd = fds[i].fd;
                    int slot = find_slot_by_fd(&clientStates, fd);
                    ssize_t bytes_read = read(fd, &clientStates[slot].buffer, sizeof(clientStates[slot].buffer));
                    if (bytes_read <= 0)
                    {
                        close(fd);
                        if (slot == -1)
                        {
                            printf("Tried to close fd that doesnt exist?\n");
                        }
                        else
                        {
                            clientStates[slot].fd = -1;
                            clientStates[slot].state = STATE_DISCONNECTED;
                            printf("Client disconnected or error\n");
                            nClientFds--;
                        }
                    }
                    else
                    {
                        printf("Receveied data from client: %s\n", clientStates[slot].buffer);
                        handle_client(&clientStates[slot]);
                        //handle_client(*slot)

                    }
                }
            }
        }
    }
}
