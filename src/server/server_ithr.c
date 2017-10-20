/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "job.h"
void* parse_message(MESSAGE* poped_mes){
    write(poped_mes->socket_id, poped_mes->text, poped_mes->text_len);
    fflush(stdout);
    free(poped_mes);
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
            printf("\t\t\t\t [MESSAGE] poped!\n");
            parse_message((MESSAGE*)poped_mes);
            printf("\t\t\t\t [MESSAGE] sended!\n");
        }
    }
}
