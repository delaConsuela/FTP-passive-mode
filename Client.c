#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "modules.c"


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

    printf("ok\n");

    do{
        rc = commands(sd_connection);
        printf("valore resitituito %d\n", rc);
    } while(rc != 1);

    close(sd_connection);
    printf("[+]client ended succesfully\n");

    return 0;
}