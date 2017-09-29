/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/select.h>
#include <string.h>

void printids(){
    pid_t       pid;
    pthread_t   tid;

    pid = getpid();
    tid = pthread_self();
    printf("pid: %lu , tid: %lu(0x%lx)\n", (unsigned long)pid, (unsigned long)tid, (unsigned long)tid);
}

void* open_server(){
    int welcomeSocket, newSocket;
    char send_buffer[1024];
    char recv_buffer[1024];
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    unsigned int i;
    struct timeval timer;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(7891);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* Set all bits of the padding field to 0 */
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

    // * fd_set - 소켓관리셋생성
    fd_set fdSet, tempFdSet;
    int fdNum, fd_array[FD_SETSIZE], fd_array_max;
    fd_array_max = 0;
    // - fd_set 초기화
    FD_ZERO(&fdSet);
    // - fd_set에 listen 소켓 지정번호를 추가
    FD_SET(welcomeSocket, &fdSet);

    // 연결대기 정보변수 선언
//    SOCKADDR_IN clientAddr; //////////// serverAddr
//    int addrLen = sizeof(SOCKADDR_IN);
    addr_size = sizeof serverStorage;
    memset(&serverAddr, 0, addr_size);
//    SOCKET clientSocket; newSocket

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

        for (i = 0; i < fd_array_max; i++){
            int savedSocket = fd_array[i]; //tempFdSet.__fds_bits[i];
            /*---- Send message to the socket of the incoming connection ----*/
            if (FD_ISSET(savedSocket, &tempFdSet) ){
                int income = recv(savedSocket, recv_buffer,1024,0);
                if (income >0){
                    printf("Client received from %d: %s\n",savedSocket, recv_buffer);
/*                    if ( !strncmp(recv_buffer, "close", 5) )
                        printf("close?");
*/
                    int outgo = send(savedSocket,send_buffer,13,0);
                    if (outgo >0){
                        strcpy(send_buffer,"Got it\n");
                        printf("Send message to Client %d : %s\n", savedSocket, send_buffer);
                        memset(send_buffer, '\0', 1024);
                    }
                    memset(recv_buffer, '\0', 1024);
                } else {
                    close(savedSocket);
                    printf("closed %d\n",savedSocket);
                    FD_CLR(savedSocket, &fdSet);
                }
            } /*else select new socket*/
        }
    }
//    shutdown(welcomeSocket,2);
    return (void *)0;
}

int main(){
    int thread_err;
    int status;

    printids();
    pthread_t cthr, wthr;

    if (thread_err = pthread_create(&cthr, NULL, open_server, NULL) ){
        perror("thread create error:");
        exit(0);
    }
    pthread_join(cthr, (void **)&status);
    return 1;
}
