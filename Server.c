#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "modules.c"

int main(){
    int sd_server, sd_connection, rc;
    int sd_server_data, sd_connection_data; 
    int portaCalcolata;
    struct sockaddr_in address, address_data, client_addr, client_addr_data;
    socklen_t size_addr;
    unsigned short portaCondivisa;
 

    //////////////////////          CONNESSIONE          //////////////////////
    
    sd_server = socket(AF_INET, SOCK_STREAM, 0);
    if(sd_server < 0){
        printf("[-]socket creation failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]creation OK\n");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    rc = bind(sd_server, (struct sockaddr *) &address, sizeof(address));
    if(rc < 0){
        printf("[-]bind failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]bind OK\n");
    printf("listing on port %d ..\n", PORT);

    rc = listen(sd_server, MAX_CON);
    if(rc < 0){
        printf("[-]listen failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]listen OK\n");

    size_addr = sizeof(struct sockaddr_in);
    sd_connection = accept(sd_server, (struct sockaddr *)&client_addr, &size_addr);
    if(sd_connection < 0){
        printf("[-]accept failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]accept OK\n");

    //////////////////////      CONNESSIONE DATI      //////////////////////

    sd_server_data = socket(AF_INET, SOCK_STREAM, 0);
    if(sd_server_data < 0){
        printf("[-]socket creation failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]creation OK\n");

    printf("\ndata port negotiation..\n");

    portaCondivisa = client_addr.sin_port;
    printf("[+]port %hu\n", portaCondivisa);
    portaCondivisa = DataPort(portaCondivisa);
    printf("[+]data port %hu\n", portaCondivisa);

    address_data.sin_family = AF_INET;
    address_data.sin_addr.s_addr = htonl(INADDR_ANY);
    address_data.sin_port = htons(portaCondivisa);
        
    sprintf(send_buffer, "%d", portaCondivisa);
    sending(sd_connection, send_buffer);

    rc = bind(sd_server_data, (struct sockaddr *) &address_data, sizeof(address_data));
    if(rc < 0){
        printf("[-]bind failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]bind OK\n");
    printf("listing on port %d ..\n", portaCondivisa);

    rc = listen(sd_server_data, MAX_CON);
    if(rc < 0){
        printf("[-]listen failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]listen OK\n");

    size_addr = sizeof(struct sockaddr_in);
    sd_connection_data = accept(sd_server_data, (struct sockaddr *)&client_addr_data, &size_addr);
    if(sd_connection_data <= 0){
        printf("[-]accept failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]accept OK\n");

    do{

        rc = server_command(sd_connection, sd_connection_data);
       
    } while(rc != 1);

    
    close(sd_connection);
    close(sd_connection_data);
    close(sd_server);
    close(sd_server_data);

    return 0;
}
