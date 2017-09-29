#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(){
    int clientSocket;
    char recv_buffer[1024];
    char send_buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(7891);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*---- Connect the socket to the server using the address struct ----*/
    addr_size = sizeof serverAddr;

    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
    while(1){
        scanf("%s",send_buffer);

        /*---- Send the message from the user input into the send_buffer ----*/
        send(clientSocket, send_buffer, 13,0);
        printf("Data sended: %s\n", send_buffer);

        /*---- Read the message from the server into the recv_buffer ----*/
        recv(clientSocket, recv_buffer, 1024, 0);
        printf("Data received: %s\n",recv_buffer);

        memset(send_buffer, '\0', 1024);
        memset(recv_buffer, '\0', 1024);
    }
    return 0;
}
