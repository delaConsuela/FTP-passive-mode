#include <string.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define PORT        3000
#define BUFF_LEN    2000
#define MAX_CON     5
#define CMD_LEN     10

void get_file(int sd_connection);
void put_file(int sd_connection);

void client_recv_file(int sd_connection);
void server_recv_file(int sd_connection);

int server_command(int sd_connection, int sd_connection_data);
int commands(int sd_connection, int sd_connection_data);

void sending(int sd_connection, char *send_buffer);
void receiving(int sd_connection, char *recv_buffer);

//contengono il dato
char temp_buffer_send[BUFF_LEN];
char temp_buffer_recv[BUFF_LEN];

//usati per il trasferimento
char send_buffer[BUFF_LEN];
char recv_buffer[BUFF_LEN];

int numeroDownload = 0;
int numeroUpload = 0;
