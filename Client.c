#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "modules.c"

/*
void recv_file(int sd_connection, char *send_buffer, char *recv_buffer);
void commands(int sd_connection, char *send_buffer, char *recv_buffer);
*/


int main(){
    int sd_connection, rc, file_des;
    struct sockaddr_in srv_addr; 

    sd_connection = socket(AF_INET, SOCK_STREAM, 0);
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(PORT);

    printf("connecting to the server..\n");
    inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr);    

    if(connect(sd_connection, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) != 0){
        printf("[-]connect failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    } 
    printf("[+]connection established\n");

    //messaggio di benvenuto
    rc = recv(sd_connection, recv_buffer, BUFF_LEN, 0);
    if(rc < 0){
        printf("[-]recv failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }   
    printf("[+]message received: %s", recv_buffer);
    bzero(recv_buffer, BUFF_LEN);

    while(1){
        commands(sd_connection, send_buffer, recv_buffer);
        break;
    }

    close(sd_connection);
    printf("[+]client ended succesfully\n");

    return 0;
}