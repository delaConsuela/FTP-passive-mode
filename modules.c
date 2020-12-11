#include "modules.h"


void sending(int sd_connection, char *send_buffer){
    int rc;
    rc = send(sd_connection, send_buffer, BUFF_LEN, 0);
    if(rc < 0){
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
    if(rc < 0){
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
    if(file_des == EBADF){
        printf("[-]bad pathname, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]file opened correctly\n");

    //file_des = open("../prova.c", O_RDONLY);
    rc = sendfile(sd_connection, file_des, NULL, BUFF_LEN);
    if(rc < 0){
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
    if(fd < 0){
        printf("[-]open failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]file downloading.. \n");
        
    while(1){
        rc = recv(sd_connection, recv_buffer, BUFF_LEN, 0);
        if(rc == 0){
            control = write(fd, recv_buffer, BUFF_LEN);
            if(control < 0){
                printf("[-]write failed, error number %d\n", errno);
                exit(EXIT_FAILURE);
            }
            printf("[+]byte received %d\n", control);
            printf("[+]closing file ..\n");
            close(fd);
            return;
        }
        else if(rc < 0){
            printf("[-]recv failed, error number %d\n", errno);
            exit(EXIT_FAILURE);
        }  
    }
    return;
}


void put_file(int sd_connection){
    int rc, file_des;
    char file_name[BUFF_LEN];
    
    printf("[+]enter the name of the file: ");
    scanf("%s", file_name);

    file_des = open(file_name, O_RDONLY);
    if(file_des == EBADF){
        printf("[-]bad pathname, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]file opened correctly\n");

    //file_des = open("../prova.c", O_RDONLY);
    rc = sendfile(sd_connection, file_des, NULL, BUFF_LEN);
    if(rc < 0){
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
    if(fd < 0){
        printf("[-]open failed, error number %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("[+]file uploading.. \n");
        
    while(1){
        rc = recv(sd_connection, recv_buffer, BUFF_LEN, 0);
        if(rc == 0){
            control = write(fd, recv_buffer, BUFF_LEN);
            if(control < 0){
                printf("[-]write failed, error number %d\n", errno);
                exit(EXIT_FAILURE);
            }
            printf("[+]byte received %d\n", control);
            printf("[+]closing file ..\n");
            close(fd);
            return;
        }
        else if(rc < 0){
            printf("[-]recv failed, error number %d\n", errno);
            exit(EXIT_FAILURE);
        }  
    }
    return;
}


int server_command(int sd_connection){
    int rc;
    FILE *fp;
    char container[BUFF_LEN];

    receiving(sd_connection, recv_buffer);

    if(strcmp(temp_buffer_recv, "get\n") == 0){
        get_file(sd_connection);
    }
    
    if(strcmp(temp_buffer_recv, "put\n") == 0){
        server_recv_file(sd_connection);
    }

    if(strcmp(container, "exit\n") == 0){
        printf("[+]client closed the connection\n");
        return 1;
    }

    else{

        fp = popen(temp_buffer_recv, "r");
        if(fp == NULL){
            printf("[-]popen failed, error number %d\n", errno);
            exit(EXIT_FAILURE);
        }

        while(fgets(container, BUFF_LEN, fp) != NULL){
            printf("container %s", container);
            strcat(send_buffer, container);
        }

        printf("buffer:\n%s", send_buffer);
        sending(sd_connection, send_buffer);
        pclose(fp);
    }

}

int commands(int sd_connection){
    int rc;
    char container[BUFF_LEN];

    printf("========== COMMANDS ==========\n");
    printf("ls\ncd\nmkdir\nget\nput\nexit");
    printf("FTP> ");

    fgets(send_buffer, BUFF_LEN, stdin);
    strcpy(container, send_buffer);
    sending(sd_connection, send_buffer);
    printf("[+]waiting ..\n");

    if(strcmp(container, "get\n") == 0){
        client_recv_file(sd_connection);
    }

    if(strcmp(container, "put\n") == 0){
        put_file(sd_connection);
    }

    if(strcmp(container, "exit\n") == 0){
        printf("[+]exiting, goodbye!\n");
        return 1;
    }

    else{
        receiving(sd_connection, recv_buffer);
    }
}