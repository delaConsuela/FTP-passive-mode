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
    struct sockaddr_in address, client_addr;
    socklen_t size_addr;

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

    while(1){

        sd_connection = accept(sd_server, (struct sockaddr *)&client_addr, &size_addr);
        if(sd_connection < 0){
            printf("[-]accept failed, error number %d\n", errno);
            exit(EXIT_FAILURE);
        }
        printf("[+]accept OK\n");

        do{
            rc = server_command(sd_connection);
            printf("valore resitituito %d\n", rc);
        } while(rc != 1);

        break;
        close(sd_connection);
    }

    
    close(sd_server);

    return 0;
}