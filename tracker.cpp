#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <iostream>

using namespace std;

void* chitchat (void* client_socket)
{
    int cli = *((int*)client_socket);
    free(client_socket);

    char message[1024];

    while(true)
    {
        bzero(message, 1024);
        int valread = recv(cli , message, 1024, 0);

        if(valread == -1)
        {
            perror("Message can't be read");
            exit(-1);
        }
        printf("Client %d says : %s \n", cli, message );

        string check(message);
        if (check == "Bye" || check == "bye")
        {
            cout << "<<<<---- Closing connection with Client[" << cli << "]" << "---->>>>" << endl;
            break;
        }
    }
    
    close(cli);
    pthread_exit(NULL);
    return NULL;
}

int main()
{
    int sock, cli;
    int sent_bytes;
    unsigned int len;
    char message[1024];

    struct sockaddr_in server, client;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1)
    {
        perror("socket: ");
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(8989);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&server.sin_zero, 8);

    len = sizeof(struct sockaddr_in);

    if(bind(sock, (struct sockaddr*)&server, len) == -1)
    {
        perror("bind");
        exit(-1);
    }

    if(listen(sock, 10) == -1)
    {
        perror("listen");
        exit(-1);
    }
    cout << "<<--Tracker is now listening-->>\nWaiting for connections......\n" << endl;
    
    while(true)
    {
        int new_cli = accept(sock, (struct sockaddr*)&client, &len);

        if(new_cli == -1)
        {
            perror("accept");
            exit(-1);
        }

        cout << "Connection Established : Welcome Client[" << new_cli << "]" << endl;

        pthread_t t;
        int* p_cli = (int*)malloc(sizeof(int));
        *p_cli = new_cli;
        pthread_create(&t, NULL, chitchat, p_cli);
    }
}