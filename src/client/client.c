#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "client.h"
#include "job.h"

void prepare_file(char* user_id, char* file_name, int clientSocket){
    int     total_file_len, num_stripe;
    char    read_buffer[1];
    char    put_req_buffer[256];
    FILE*   prep_fd = fopen(file_name, "rb");
    int     i = 0;
    BOX*    data_box = malloc(sizeof(BOX));
    char*   data = malloc(BOX_SIZE);

    fseek(prep_fd,0,SEEK_END);
    total_file_len = ftell(prep_fd);
    num_stripe = total_file_len / (BOX_SIZE);
    (total_file_len % BOX_SIZE) != 0 ? num_stripe++ : num_stripe;
    fseek(prep_fd,0,SEEK_SET);
    /*printf("opened: %d \n", total_file_len);*/

    memcpy(put_req_buffer,"put: ",5);
    memcpy(put_req_buffer+sizeof(char)*5, &num_stripe, sizeof(int));
    memcpy(put_req_buffer+sizeof(char)*5+sizeof(int), file_name, 128);
    send(clientSocket, put_req_buffer, 256, 0);

    while(i < num_stripe){
        int     data_len = BOX_SIZE;
        size_t  read_len;

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
            printf("[err] when read %d %d\n", (int)data_len, (int)read_len);
            exit(3);
        } else {
            send(clientSocket, data_box , sizeof(BOX), 0);
            send(clientSocket, data , data_len, 0);
           }
        i++;
    }
    recv(clientSocket, read_buffer, 1, 0);
    //printf("%s\n",read_buffer);
    if( memcmp(read_buffer,"T",1)==0){
        printf("[FILE: %s] offset: %d out of %d send success.\n", file_name, i, num_stripe);
    } else if( memcmp(read_buffer,"F",1)==0){
        printf("[FILE: %s] offset: %d out of %d send FAILED.\n", file_name, i, num_stripe);
    } else{
        printf("     ERROR FILE SEND\n");
    }
    free(data_box);
    free(data);
    fclose(prep_fd);
}

void prepare_list(char* user_id, int clientSocket){
    char    get_req_buffer[32];
    char    list_buffer[BOX_SIZE * 8];

    memcpy(get_req_buffer,"lst: ",5);
    memcpy(get_req_buffer+sizeof(char)*5, user_id, 8);
    send(clientSocket, get_req_buffer, 32, 0);
    fflush(stdout);
    recv(clientSocket, list_buffer, BOX_SIZE*8,0);
    fflush(stdin);
    printf("%s\n", list_buffer);
}

void request_get(char* user_id, char* file_name, int clientSocket){
    char    get_buffer[256];
    BOX*    recv_meta = malloc(sizeof(BOX));
    char*   recv_data = malloc(BOX_SIZE);
    memcpy(get_buffer,"get: ",5);
    memcpy(get_buffer+sizeof(char)*5, user_id, 8);
    memcpy(get_buffer+sizeof(char)*(5+8), file_name, 128);
    /* SEND USERID FILENAME */
    send(clientSocket, get_buffer, 256, 0);
    fflush(stdout);
    /* GET META INFO */
    int     i, max_piece;
    char*   max_pieces = malloc(8);
    recv(clientSocket, max_pieces, 8, 0);
    fflush(stdin);
    max_piece = atoi(max_pieces);

    uint16_t    offset;
    uint32_t    data_len;
    FILE*       write_fp;
    char        addr[80];
    printf("max: %d\n",max_piece);
    for(i=0; i<max_piece; i++){
        printf("i: %d\n",i);
        recv(clientSocket, recv_meta, sizeof(BOX), 0 );
        if (i==0) {
            /*meta parse*/
            printf("recieved: %s's %s\n", user_id, file_name);
            sprintf(addr, "%s%s", DOWNLOAD, file_name);
            printf("writing to : %s\n",addr);
            write_fp = fopen(addr, "w+");
        }
        offset = recv_meta->file_offset;
        data_len = recv_meta->data_len;

        recv(clientSocket, recv_data, BOX_SIZE, 0 );
        fflush(stdin);
        /*file writing*/
        fseek(write_fp, BOX_SIZE * offset, SEEK_SET);
        fwrite(recv_data, sizeof(char), data_len, write_fp);
        printf("recv_data: %s\n",recv_data);
        fflush(stdout);
        /*buffer cleaning*/
        memset(recv_meta,'\0',sizeof(BOX));
        memset(recv_data,'\0',sizeof(BOX_SIZE));
    }
    fclose(write_fp);
    printf("writing done %s\n",addr);
}


int main(){
    int     clientSocket;
    char    recv_buffer[1024];
    char    send_buffer[1024];
    char    user_id[8];
    char    file_name[128];
    struct  sockaddr_in serverAddr;
    socklen_t addr_size;

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
    scanf("%9s",user_id);
    fflush(stdin);

    char action[3];
    while(1){
        printf("NEW ACTION: ");
        scanf("%9s",action);
        fflush(stdin);
        if (strcmp(action,"put")==0){
            printf("PUT FILE: ");
            scanf("%9s",file_name);
            fflush(stdin);
            printf("...... processing\n");
            prepare_file(user_id, file_name, clientSocket);
        } else if (strcmp(action,"list")==0){
            printf("...... processing\n");
            prepare_list(user_id, clientSocket);
        } else if (strcmp(action,"get")==0){
            printf("GET FILE: ");
            scanf("%9s",file_name);
            fflush(stdin);
            printf("...... processing\n");
            request_get(user_id, file_name, clientSocket);
        } else {
            printf("      UNKNOWN ACTION... %9s\nput , get , list\n",action);
            continue;
        }
        memset(action,      '\0', 3);
        memset(file_name,   '\0', 128);
        memset(send_buffer, '\0', 1024);
        memset(recv_buffer, '\0', 1024);
    }
    return 0;
}
