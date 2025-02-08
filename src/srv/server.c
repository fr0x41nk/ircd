#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>


int main() {

    struct sockaddr_in serverInfo = {0};
    struct sockaddr_in clientInfo = {0};
    


    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Socket");
        return -1;
    }

    //bind

    if (bind(fd, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) == -1) {
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

    



}

