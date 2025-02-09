#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include "srvpoll.h"
#include <unistd.h>
#include <string.h>

void handle_client(int fd){
    char buf[5000] = "Hello client!";

    write(fd,buf, strlen(buf) );

}



int main() {

    struct sockaddr_in server_addr, client_addr = {0};
    //struct sockaddr_in clientInfo = {0};
   // int clientSize = 0;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = 0;
    server_addr.sin_port = htons(PORT);
    //unsigned int clientSize = 0;
    socklen_t clientSize = sizeof(client_addr);



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
    
    //int cfd = -1;

    fds[0].fd = fd;
    fds[0].events = POLLIN;
    //fds[1].fd = -1; //init client fd
    //loop to init all fds until MAX_CLIENTS
    for(int i = 1; i < MAX_CLIENTS;i++) {
        fds[i].fd = -1;
    }



    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int)) == -1) {
        perror("setsockopt");
    }



    while (1) {



        int n_events = poll(fds,MAX_CLIENTS, -1); //her polles alle FD laget i structen
        //On success, poll() returns a nonnegative
        //value which is the number of elements in the pollfds whose revents fields have been set to a 
        //nonzero value

        if (n_events > 0) {
            if (fds[0].revents & POLLIN) {
                int cfd = accept(fd, (struct sockaddr*)&client_addr,&clientSize);

                //end test
                fds[1].fd = cfd;
                fds[1].events = POLLIN;
            
        }
        if (fds[1].fd != -1 && fds[1].revents & POLLIN) {
            printf("In after handle client");
            handle_client(fds[1].fd);

            close(fds[1].fd);
            fds[1].fd = -1; //init client fd


        }
    } else if (n_events == 0) {
        printf("Timeout!");
        return 0;
    } else {
        printf("Error occured\n");
    }
}

}
