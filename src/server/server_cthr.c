/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/select.h>
#include <string.h>
#include <signal.h>
#include "server_wthr.h"
#include "server_ithr.h"

int WELCOME_INT[FD_SETSIZE];
int WELCOME_SOCKET;
void SignalHandler(int signal){
    int i;
    for(i=0;i<FD_SETSIZE;i++)
        if(WELCOME_INT[i] != 0)
            close(WELCOME_INT[i]);
    close(WELCOME_SOCKET);
    printf("\nSIG CLOSING...\n");
    exit(0);
}

void printids(){
    pid_t       pid;
    pthread_t   tid;

    pid = getpid();
    tid = pthread_self();
    printf("pid: %lu , tid: %lu(0x%lx)\n", (unsigned long)pid, (unsigned long)tid, (unsigned long)tid);
}

void job_lst(char* recv_buffer, int savedSocket, JOB_Q* job_q){
    int income;
    JOB* new_job = malloc(sizeof(JOB));
    new_job->work = JOB_LIST;
    new_job->pieces = 0;
    new_job->file_num=0;
    memcpy(new_job->user_id, recv_buffer+sizeof(char)*5,sizeof(char)*8);
    new_job->socket_id = savedSocket;
    insert_q(job_q, (void*) new_job);
}

void job_get(char* recv_buffer, int savedSocket, JOB_Q* job_q){
    int income;
    JOB* new_job = malloc(sizeof(JOB));
    BOX* tmp_box = malloc(sizeof(BOX));
    new_job->work     = JOB_GET;
    new_job->pieces   = 0;
    new_job->file_num = 0;

    memcpy(new_job->user_id, recv_buffer+sizeof(char)*5,sizeof(char)*8);
    memcpy(tmp_box->file_name, recv_buffer+sizeof(char)*(5+8), 128);
    new_job->socket_id = savedSocket;
    new_job->data_piece = tmp_box;
    insert_q(job_q, (void*) new_job);
}

void job_put(char* recv_buffer, int savedSocket, JOB_Q* job_q ){
    int num_stripe;
    int i;
    memcpy(&num_stripe, recv_buffer+sizeof(char)*5,sizeof(int));

    BOX* income_boxes = malloc(sizeof(BOX*)*num_stripe);
    BOX tmp_boxes[num_stripe];
    for (i=0;i<num_stripe;i++){
        BOX* data_box = malloc(sizeof(BOX));
        char* data = malloc(sizeof(char)*BOX_SIZE);
        int box_income = recv(savedSocket, data_box, sizeof(BOX), 0);
        int data_income = recv(savedSocket, data, sizeof(char)*data_box->data_len, 0);
        if(data_income > 0){
            data_box->data = data;
            printf("<Data received Client[%d] USER[%s] FILE[%s]>\n%s\n.\n",savedSocket, data_box->user_id, data_box->file_name, data_box->data);
            tmp_boxes[i] = *data_box;
        } else {
            printf("Send message to Client(%d) : %s\n", savedSocket, "F");
        }
    }
    income_boxes = tmp_boxes;
/*    for (i=0;i<num_stripe;i++){
        BOX tmp_box = income_boxes[i];
        printf("offset: %d\n",tmp_box.file_offset);
        printf("len: %x\n",tmp_box.data_len);
        printf("data: %s\n", tmp_box.data);
    }*/
    JOB* new_job = malloc(sizeof(JOB));
    new_job->work = JOB_PUT;
    new_job->pieces = num_stripe;
    new_job->socket_id = savedSocket;
    new_job->data_piece = income_boxes;
    insert_q(job_q, (void*) new_job);
}

void* open_server(){
    int welcomeSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    unsigned int i;
    struct timeval timer;

    pthread_t wthr;
    pthread_t ithr;
    int wthread_err;
    int ithread_err;

    JOB_Q* this_job_q;
    JOB_Q* this_mes_q;

    char recv_buffer[256];
    char file_name[128];

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*---- Configure settings of the server address struct ----*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*---- Bind the address struct to the socket ----*/
    bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    while(1){
        /*---- Listen on the socket, with 5 max connection requests queued ----*/
        if(listen(welcomeSocket,5)==0) {
            printf("Listening\n");
            printids();
            break;
        }
        else
            printf("Error\n");
    }
    WELCOME_SOCKET=welcomeSocket;
    // * fd_set - 소켓관리셋생성
    fd_set fdSet, tempFdSet;
    int fdNum, fd_array[FD_SETSIZE], fd_array_max;
    fd_array_max = 0;
    FD_ZERO(&fdSet);
    FD_SET(welcomeSocket, &fdSet);

    // 연결대기 정보변수 선언
    addr_size = sizeof serverStorage;
    memset(&serverAddr, 0, addr_size);

    /*JOB Q create*/
    this_job_q        = create_job_q();
    this_mes_q        = create_job_q();
    this_job_q->mes_q = this_mes_q;

    /*- wthr create -*/
    if (wthread_err = pthread_create(&wthr, NULL, wthr_main, (void*)this_job_q) ){
        perror("w thread create error:");
        exit(0);
    }
    /*- ithr create -*/
    if (ithread_err = pthread_create(&ithr, NULL, ithr_main, (void*)this_mes_q) ){
        perror("i thread create error:");
        exit(0);
    }

    while(1){
        tempFdSet = fdSet;
        fdNum = select(FD_SETSIZE, &tempFdSet, NULL, NULL, NULL);
        if (fdNum ==0 ){
            printf(" No Welcome Socket Connection... Sleep 1 retry\n ");
            sleep(1);
            continue;
        }
        if (FD_ISSET(welcomeSocket, &tempFdSet)){
             /*---- Accept call creates a new socket for the incoming connection ----*/
            newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
            if (newSocket == 0){
                printf("Error - Fail to handshake\n");
                continue;
            } else {
                printf("Handshake %d\n",newSocket);
            }
            FD_SET(newSocket, &fdSet);
            fd_array[fd_array_max++] = newSocket;
            puts("accept");
            continue;
        }
        *WELCOME_INT = *fd_array;
        for (i = 0; i < fd_array_max; i++){
            int savedSocket = fd_array[i];
            /*---- Send message to the socket of the incoming connection ----*/
            if (FD_ISSET(savedSocket, &tempFdSet) ){
                int income = recv(savedSocket, recv_buffer,256,0);

                if (income >0){
                    memcpy(file_name, recv_buffer + sizeof(char)*5 + sizeof(int), 128);

                    if( memcmp(recv_buffer,"put",3)==0 ){
                        /*PUT*/
                        job_put(recv_buffer, savedSocket, this_job_q);
                    } else if ( memcmp(recv_buffer, "lst", 3)==0){
                        /*LIST*/
                        job_lst(recv_buffer, savedSocket, this_job_q);
                    } else if ( memcmp(recv_buffer, "get", 3)==0){
                        /*GET*/
                        job_get(recv_buffer, savedSocket, this_job_q);
                    } else {
                        printf("\n<Network Error>\n");
                        exit(0);
                    }
                } else {
                    close(savedSocket);
                    //printf("closed %d\n",savedSocket);
                    FD_CLR(savedSocket, &fdSet);
                }
                memset(recv_buffer, '\0', 256);
                memset(file_name, '\0', 128);
            } /*else select new socket*/
        }
    }
    return (void *)0;
}

int main(){
    int thread_err;
    int status;

    signal(SIGINT, SignalHandler);
    //printids();
    pthread_t cthr;

    if (thread_err = pthread_create(&cthr, NULL, open_server, NULL) ){
        perror("c thread create error:");
        exit(0);
    }
    pthread_join(cthr, (void **)&status);
    return 1;
}


