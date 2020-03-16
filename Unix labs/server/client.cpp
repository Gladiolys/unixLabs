#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void handleError(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int requestSocketFd, port, writenOrReadLength;
    struct sockaddr_in serverAddress;
    struct hostent *server;

    char buffer[256];

    if (argc < 3) {
        port = 50000;
        server = gethostbyname("localhost");
    } else {
        port = atoi(argv[2]);
        server = gethostbyname(argv[1]);
    }

    requestSocketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (requestSocketFd < 0)
        handleError("ERROR opening socket");


    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);

    serverAddress.sin_port = htons(port);

    if (connect(requestSocketFd,(struct sockaddr *) &serverAddress,sizeof(serverAddress)) < 0)
        handleError("ERROR connecting");

    while (true) {
        printf("Please enter the message: ");

        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        writenOrReadLength = write(requestSocketFd, buffer, strlen(buffer));

        if (writenOrReadLength < 0)
            handleError("ERROR writing to socket");

        bzero(buffer, 256);
        writenOrReadLength = read(requestSocketFd, buffer, 255);

        if (writenOrReadLength < 0)
            handleError("ERROR reading from socket");

        printf("Read from server: %s\n", buffer);
    }

    close(requestSocketFd);
    return 0;
}
