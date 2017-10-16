#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "job.h"

void prepare_file(char* user_id, char* file_name, int clientSocket){
    int total_file_len, num_stripe;
    char read_buffer[1];
    char put_req_buffer[256];
    FILE* prep_fd = fopen(file_name, "rb");
    int i = 0;

    BOX* data_box = malloc(sizeof(BOX));
    char* data = malloc(BOX_SIZE);

    fseek(prep_fd,0,SEEK_END);
    total_file_len = ftell(prep_fd);
    num_stripe = total_file_len / (BOX_SIZE);
    fseek(prep_fd,0,SEEK_SET);
    /*printf("opened: %d \n", total_file_len);*/

    memcpy(put_req_buffer,"put: ",5);
    memcpy(put_req_buffer+sizeof(char)*5, &num_stripe, sizeof(int));
    memcpy(put_req_buffer+sizeof(char)*5+sizeof(int), file_name, 128);
    send(clientSocket, put_req_buffer, 256, 0);

    while(i < num_stripe){
        int data_len = BOX_SIZE;
        size_t read_len;

        if (total_file_len - (i + 1) * BOX_SIZE < 0)
            data_len = total_file_len - i * BOX_SIZE;
        if (data_len == 0)
            break;

        memcpy(data_box->user_id, user_id, 8);
        memcpy(data_box->file_name, file_name, 128);
        data_box->file_offset = (uint16_t) i;
        data_box->data_len    = data_len;
        read_len = fread(data, 1, data_len, prep_fd);
        /*printf("read %d %d\n", (int)data_len, (int)read_len);*/
        if(read_len != data_len){
            printf("err when read %d %d\n", (int)data_len, (int)read_len);
            exit(3);
        } else {
            send(clientSocket, data_box , sizeof(BOX), 0);
            send(clientSocket, data , data_len, 0);
            recv(clientSocket, read_buffer, 1, 0);
            printf("%s\n",read_buffer);
            if( memcmp(read_buffer,"T",1)==0){
                printf("[FILE: %s] offset: %d out of %d send success.\n", file_name, i, num_stripe);
            } else if( memcmp(read_buffer,"F",1)==0){
                printf("[FILE: %s] offset: %d out of %d send FAILED.\n", file_name, i, num_stripe);
            } else{
                printf("     ERROR FILE SEND\n");
            }
        }
        i++;
    }
    free(data_box);
    free(data);
    fclose(prep_fd);
}

int main(){
    int clientSocket;
    char recv_buffer[1024];
    char send_buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    char user_id[8];
    char file_name[128];

    /*---- Create the socket. The three arguments are:
       1) Internet domain 2) Stream socket 3) Default protocol
       (TCP in this case)                                   ----*/
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*---- Configure settings of the server address struct ----*/
    serverAddr.sin_family = AF_INET;   /* Address family = Internet */
    serverAddr.sin_port = htons(7891); /* Set port number */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); /* Set IP address */
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*---- Connect the socket to the server using the address struct ----*/
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

    printf("user_id: ");
    scanf(" %9s",user_id);
    fflush(stdin);

    char action[3];
    while(1){
        scanf("%9s",action);
        if (strcmp(action,"put")==0){
            printf("FILE: ");
            scanf("%9s",file_name);
            prepare_file(user_id, file_name, clientSocket);
        } else if (strcmp(action,"get")==0){
            printf("FILE: ");
            scanf("%9s",send_buffer);
        } else {
            printf("      UNKNOWN ACTION... %9s\n",action);
            continue;
        }

        /*---- Read the message from the server into the recv_buffer ----*/

        memset(send_buffer, '\0', 1024);
        memset(recv_buffer, '\0', 1024);
    }
    return 0;
}
