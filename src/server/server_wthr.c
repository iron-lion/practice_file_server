/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "job.h"

void* parse_job(char* recv_message){
    JOB* new_job;
    new_job = malloc(sizeof(JOB));

}


void* wthr_main(void* job_q){ /*wthr open*/
    void* poped_job;
    JOB_Q* this_job_q = (JOB_Q*) job_q;


    while(1){
        if (this_job_q->jobs->count == 0){
            sleep(1);
            printf("No job... \n");
            continue;
        } else {
            sleep(1);
            printf("Yes job... \n");
            //poped_job = pop_q((JOB_Q*)this_job_q);
        }
    }
}


