/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "job.h"
void* parse_message(MESSAGE* poped_mes){
    write(poped_mes->socket_id, poped_mes->text, poped_mes->text_len);
    fflush(stdout);
}

void* ithr_main(void* mes_q){
    void* poped_mes;
    JOB_Q* this_mes_q = (JOB_Q*) mes_q;

    while(1){
        if (this_mes_q->jobs->count == 0){
            printf("\t\t\tNo Message... \n");
            sleep(2);
            continue;
        } else {
            sleep(1);
            printf("\t\t\tYes Message... \n");
            poped_mes = pop_q(this_mes_q);
            printf("\t\t\tpop!\n");
            parse_message((MESSAGE*)poped_mes);
            printf("\t\t\tmessage sended!\n");
        }
    }
}
