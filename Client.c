#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "modules.c"


int main(){
    int sd_connection, rc, file_des;
    int sd_connection_data, portaCondivisa;
    struct sockaddr_in srv_addr, srv_addr_data; 

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

    receiving(sd_connection, recv_buffer);
    portaCondivisa = atoi(temp_buffer_recv);

    //////////////////////      CONNESSIONE DATI      //////////////////////
    
    sd_connection_data = socket(AF_INET, SOCK_STREAM, 0);
    srv_addr_data.sin_family = AF_INET;
    srv_addr_data.sin_port = htons(portaCondivisa);

    printf("connecting to the data channel..\n");
    inet_pton(AF_INET, "127.0.0.1", &srv_addr_data.sin_addr);    

    if(connect(sd_connection_data, (struct sockaddr *)&srv_addr_data, sizeof(srv_addr_data)) != 0){
        printf("[-]connect failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    } 
    printf("[+]connection established\n");


    do{

        rc = commands(sd_connection, sd_connection_data);
        
    } while(rc != 1);

    close(sd_connection);
    close(sd_connection_data);
    printf("[+]client ended succesfully\n");

    return 0;
}