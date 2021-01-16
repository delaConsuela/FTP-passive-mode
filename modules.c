#include "modules.h"
#include <sys/types.h>
#include <sys/stat.h>

unsigned short DataPort(unsigned short p){
    unsigned short x, y;
    x = p%1000;
    y = p/1000;
    return (x * 256) + y;
}

void sending(int sd_connection, char *send_buffer){
    int rc;
    rc = send(sd_connection, send_buffer, BUFF_LEN, 0);
    if (rc < 0){
        printf("[-]send failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]send OK\n");
    strcpy(temp_buffer_send, send_buffer);
    bzero(send_buffer, BUFF_LEN);
}

void receiving(int sd_connection, char *recv_buffer){
    int rc;
    rc = recv(sd_connection, recv_buffer, BUFF_LEN, 0);
    if (rc < 0){
        printf("[-]recv failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]message received: %s", recv_buffer);
    strcpy(temp_buffer_recv, recv_buffer);
    bzero(recv_buffer, BUFF_LEN);
}

void get_file(int sd_connection){
    int rc, file_des;
    char file_name[BUFF_LEN];

    printf("[+]waiting file name ..\n");
    receiving(sd_connection, recv_buffer);
    strcpy(file_name, temp_buffer_recv);

    file_des = open(file_name, O_RDONLY);
    if (file_des < 0){
        printf("[-]open failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]file opened correctly, file descriptor %d\n", file_des);

    rc = sendfile(sd_connection, file_des, NULL, BUFF_LEN);
    if (rc < 0){
        printf("[-]sendfile failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]number of bytes sent %d\n", rc);
    close(file_des);
}

void client_recv_file(int sd_connection){
    int rc, fd, control = 0;
    char file_name[BUFF_LEN];

    printf("[+]enter the name of the file: ");
    scanf("%s", send_buffer);
    sending(sd_connection, send_buffer);
    strcpy(file_name, temp_buffer_send);

    fd = open("New_Download", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0){
        printf("[-]open failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]file downloading.. \n");

    while (1){
        rc = recv(sd_connection, recv_buffer, BUFF_LEN, 0);

        printf("[+]client recv_file rc value = %d\n", rc);

        if (rc > 0){

            control = write(fd, recv_buffer, BUFF_LEN);
            if (control < 0){
                printf("[-]write failed, error number %d\n", errno);
                exit(EXIT_FAILURE);
            }

            printf("[+]byte received %d\n", control);
            printf("[+]closing file ..\n");
            close(fd);
            break;
        }
        else if (rc < 0){
            printf("[-]recv failed, error number %d\n", errno);
            exit(EXIT_FAILURE);
        }
    }
}

void put_file(int sd_connection){
    int rc, file_des;
    char file_name[BUFF_LEN];

    printf("[+]enter the name of the file: ");
    scanf("%s", file_name);

    file_des = open(file_name, O_RDONLY);
    if (file_des < 0){
        printf("[-]open failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]file opened correctly\n");

    rc = sendfile(sd_connection, file_des, NULL, BUFF_LEN);
    if (rc < 0){
        printf("[-]sendfile failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]number of bytes sent %d\n", rc);
    close(file_des);
}

void server_recv_file(int sd_connection){
    int rc, fd, control = 0;
    char file_name[BUFF_LEN];

    printf("[+]server receving files.. \n");

    fd = open("New_Upload", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0){
        printf("[-]open failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]file uploading.. \n");

    while(1){
        rc = recv(sd_connection, recv_buffer, BUFF_LEN, 0);
        if (rc > 0){
            control = write(fd, recv_buffer, BUFF_LEN);
            if (control < 0){
                printf("[-]write failed, error number %d\n", errno);
                exit(EXIT_FAILURE);
            }
            printf("[+]byte received %d\n", control);
            printf("[+]closing file ..\n");
            close(fd);
            break;
        }
        else if (rc < 0){
            printf("[-]recv failed, error number %d\n", errno);
            exit(EXIT_FAILURE);
        }
    }
}

int server_command(int sd_connection, int sd_connection_data){
    int rc;
    FILE *fp;
    char container[BUFF_LEN];

    rc = fflush(stdin);
    if(rc != 0){
        printf("[-]fflush failed, error number %d\n", errno);
        EXIT_FAILURE;
    }
    printf("[+]fflush OK\n");

    printf("[+]server waiting for commands\n");
    strcpy(recv_buffer, "");
    receiving(sd_connection, recv_buffer);

    if (strcmp(temp_buffer_recv, "") != 0){

        if (strcmp(temp_buffer_recv, "get\n") == 0){
            get_file(sd_connection_data);
            return 2;
        }

        else if (strcmp(temp_buffer_recv, "put\n") == 0){
            server_recv_file(sd_connection_data);
            return 2;
        }

        else if (strcmp(temp_buffer_recv, "exit\n") == 0){
            printf("[+]client closed the connection\n");
            return 1;
        }

        else{

            fp = popen(temp_buffer_recv, "r");
            if (fp == NULL){
                printf("[-]popen failed, error number %d\n", errno);
                exit(EXIT_FAILURE);
            }

            while (fgets(container, BUFF_LEN, fp) != NULL){
                printf("container %s", container);
                strcat(send_buffer, container);
            }

            printf("buffer:\n%s", send_buffer);
            sending(sd_connection_data, send_buffer);
            pclose(fp);
            return 2;
        }
    }

    return 2;
}

int commands(int sd_connection, int sd_connection_data){
    int rc;
    char container[BUFF_LEN];

    rc = fflush(stdin);
    if(rc != 0){
        printf("[-]fflush failed, error number %d\n", errno);
        EXIT_FAILURE;
    }
    printf("[+]fflush OK\n");

    printf("\n========== COMMANDS ==========\n");
    printf("ls\ncd\nmkdir\nget\nput\nexit\n");
    printf("FTP: ");

    fgets(container, BUFF_LEN, stdin);  

    if(strcmp(container, "") == 0 || strcmp(container, "\n") == 0){
        printf("enter a new string\n");
        printf("FTP: ");
        fgets(container, BUFF_LEN, stdin);  
    }

    strcpy(send_buffer, container);
    sending(sd_connection, send_buffer);
    printf("[+]waiting ..\n");

    if (strcmp(container, "get\n") == 0){
        client_recv_file(sd_connection_data);
        return 2;
    }

    else if (strcmp(container, "put\n") == 0){
        put_file(sd_connection_data);
        return 2;
    }

    else if (strcmp(container, "exit\n") == 0){
        strcpy(send_buffer, "exit");
        sending(sd_connection, send_buffer);
        printf("[+]exiting, goodbye!\n");
        return 1;
    }

    else{
        receiving(sd_connection_data, recv_buffer);
        return 2;
    }

    return 2;
}
