#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "modules.c"

/*
void get_file(int sd_connection, char *send_buffer, char *recv_buffer);
void server_command(int sd_connection, char *send_buffer, char *recv_buffer);
*/




int main(){
    int sd_server, sd_connection, rc; 
    struct sockaddr_in address, client_addr;
    socklen_t size_addr;

    //creating a socket
    sd_server = socket(AF_INET, SOCK_STREAM, 0);
    if(sd_server < 0){
        printf("[-]socket creation failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]creation OK\n");

    //initializing it
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);


    //bind the socket to the address and port
    rc = bind(sd_server, (struct sockaddr *) &address, sizeof(address));
    if(rc < 0){
        printf("[-]bind failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]bind OK\n");
    printf("listing on port %d ..\n", PORT);


    //max request to listen
    rc = listen(sd_server, MAX_CON);
    if(rc < 0){
        printf("[-]listen failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]listen OK\n");

    //accepting requests
    size_addr = sizeof(struct sockaddr_in);
    sd_connection = accept(sd_server, (struct sockaddr *)&client_addr, &size_addr);
    if(sd_connection < 0){
        printf("[-]accept failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]accept OK\n");

    //messaggio da inviare come benvenuto
    strcpy(send_buffer, "Welcome to Summoner's Rift FTP Server!\n");
    rc = send(sd_connection, send_buffer, BUFF_LEN, 0);
    if(rc < 0){
        printf("[-]send failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]send OK\n");
    bzero(send_buffer, BUFF_LEN);

    while(1){
        //ricezione o invio pacchetti
        server_command(sd_connection, send_buffer, recv_buffer);
        break;
    }

    close(sd_connection);
    close(sd_server);

    return 0;
}