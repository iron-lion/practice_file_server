/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "job.h"
void* parse_message(MESSAGE* poped_mes){
    switch(poped_mes->type){
        case MESSAGE_TEXT:
            write(poped_mes->socket_id, (char*)(poped_mes->text), poped_mes->text_len);
            fflush(stdout);
            break;
        case MESSAGE_LIST:
            write(poped_mes->socket_id, (char*)(poped_mes->text), BOX_SIZE*8);
            fflush(stdout);
            break;
        case MESSAGE_GET:
            write(poped_mes->socket_id, poped_mes->text, poped_mes->text_len);
            fflush(stdout);
            write(poped_mes->socket_id, ((BOX*)(poped_mes->text))->data, ((BOX*)(poped_mes->text))->data_len);
            fflush(stdout);
            break;
        default:
            printf("Wrong message type\n");
            exit(1);
        }
    free((MESSAGE*)poped_mes);
}

void* ithr_main(void* mes_q){
    void* poped_mes;
    JOB_Q* this_mes_q = (JOB_Q*) mes_q;

    while(1){
        if (this_mes_q->jobs->count == 0){
            sleep(2);
            continue;
        } else {
            poped_mes = pop_q(this_mes_q);
            printf("[I] [MESSAGE] poped!\n");
            parse_message((MESSAGE*)poped_mes);
            printf("[I] [MESSAGE] sended!\n");
        }
    }
}
