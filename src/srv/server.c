#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include "srvpoll.h"
#include <unistd.h>



int main() {

    struct sockaddr_in server_addr, client_addr = {0};
    //struct sockaddr_in clientInfo = {0};
   // int clientSize = 0;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = 0;
    server_addr.sin_port = htons(PORT);
    


    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Socket");
        return -1;
    }

    //bind

    if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(fd);
        return -1;
    }

    //listen

    if (listen(fd,0) == -1) {
        perror("Listen");
        close(fd);
        return -1;
    }

    printf("Server listening on port %d\n", PORT);

    int opt = 1;

    struct pollfd fds[MAX_CLIENTS];
    
    int cfd = -1;

    fds[0].fd = fd;
    fds[0].events = POLLIN;
    

    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int)) == -1) {
        perror("setsockopt");
    }

    while (1) {
        int ret = poll(fds,MAX_CLIENTS, -1);

        if (ret > 0) {
            if (fds[0].revents & POLLIN) {
                cfd = accept(fd, (struct sockaddr*)&clientInfo,&clientSize);
                fds[1].fd = cfd;
            }
        }
        if (fds[1].fd != -1 && fds[1].revents & POLLIN) {
            handle_client(cfd);
            close(cfd);
            fds[1].fd = -1;
        }
    } else if (ret == 0) {
        printf("Timeout!");
        return 0;

    } else {
        printf("Error occured\n");
    }
}

