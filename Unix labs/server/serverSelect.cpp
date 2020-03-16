#include <sys/types.h>
#include <sys/socket.h>
#include <ctime>
#include <netinet/in.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <set>
#include <csignal>

using namespace std;

void handleSignal(int signum) {
    printf("Caught signal %d\n",signum);
    exit(signum);
}

void handleError(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int listenSocketFd, resultLength, port;
    struct sockaddr_in serverAddress{};
    char buffer[256];

    signal(SIGINT, handleSignal);

    listenSocketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocketFd < 0)
        handleError("ERROR opening socket");

    fcntl(listenSocketFd, F_SETFL, O_NONBLOCK);

    port = argc < 2 ? 50000 : atoi(argv[1]);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(listenSocketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        handleError("ERROR binding socket");

    listen(listenSocketFd, 5);

    set<int> clientSocketSet;
    clientSocketSet.clear();

    while (true) {
        fd_set readFdSet;
        FD_ZERO(&readFdSet);
        FD_SET(listenSocketFd, &readFdSet);


        for (set<int>::iterator it = clientSocketSet.begin(); it != clientSocketSet.end(); it++)
            FD_SET(*it, &readFdSet);

        timeval timeout{};
        timeout.tv_sec = 300;
        timeout.tv_usec = 0;

        if (clientSocketSet.empty()) {
            printf("No connections, waiting 300 second and down\n");
        }

        int maxFdId = max(listenSocketFd, *max_element(clientSocketSet.begin(), clientSocketSet.end()));

        if (select(maxFdId+1, &readFdSet, nullptr, nullptr, &timeout) <= 0)
            handleError("Error on select, may be time limit end");


        if (FD_ISSET(listenSocketFd, &readFdSet)) {
            int acceptFdSocket = accept(listenSocketFd, nullptr, nullptr);

            if (acceptFdSocket < 0) {
                perror("ERROR: accept connection");
                exit(3);
            }

            printf("New connection!\n");

            fcntl(acceptFdSocket, F_SETFL, O_NONBLOCK);

            if (!clientSocketSet.empty()) {
                close(acceptFdSocket);
                printf("New connection closed, because only 1 client allow!\n");
            } else {
                clientSocketSet.insert(acceptFdSocket);
            }
        }

        for (set<int>::iterator it = clientSocketSet.begin(); it != clientSocketSet.end() && !clientSocketSet.empty(); it++) {
            if (FD_ISSET(*it, &readFdSet)) {
                resultLength = recv(*it, buffer, sizeof(buffer), 0);

                if (resultLength <= 0) {
                    close(*it);
                    clientSocketSet.erase(*it);
                    continue;
                }

                printf("Here is the message: %s\n", buffer);

                send(*it, buffer, resultLength, 0);
            }
        }
    }

    return 0;
}