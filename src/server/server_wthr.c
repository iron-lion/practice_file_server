/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "job.h"

void* put_process(uint8_t pieces, BOX* data_pieces){
    int i = 0;
    for(i; i<pieces; i++){
        printf("%d\n",i);
        BOX this_box = data_pieces[i];
        FILE *fp;
        char* user = this_box.user_id;
        char* filename = this_box.file_name;
        char str[80];
        sprintf(str, "%s%s_%s_%d",MIRROR_I,user,filename,i);
        fp = fopen( str, "wb+");
        fwrite(&this_box, sizeof(BOX), 1, fp);
        fwrite(this_box.data, sizeof(char), this_box.data_len, fp);
        memset(str,'\0',80);
        fclose(fp);
    }
}


void* parse_job(JOB* poped_job){
    uint8_t work     = poped_job->work;
    uint8_t pieces   = poped_job->pieces;
    BOX* data_pieces = poped_job->data_piece;

    switch(work){
        case JOB_PUT: put_process(pieces, data_pieces); break;
        case JOB_GET:
        default : printf("ERROR, where the work parse processes\n"); break;
    }
}


void* wthr_main(void* job_q){ /*wthr open*/
    void* poped_job;
    JOB_Q* this_job_q = (JOB_Q*) job_q;


    while(1){
        if (this_job_q->jobs->count == 0){
            printf("No job... \n");
            sleep(2);
            continue;
        } else {
            sleep(1);
            printf("Yes job... \n");
            poped_job = pop_q((JOB_Q*)this_job_q);
            printf("pop!\n");
            parse_job(poped_job);
            printf("job done!\n");
        }
    }
}


