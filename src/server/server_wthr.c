/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "job.h"
#include "python_caller.h"

void* put_process(uint8_t pieces, BOX* data_pieces){
    FILE *meta_fp;
    int i = 0;
    char* user;
    char* filename;

    for(i; i<pieces; i++){
        BOX this_box[pieces];
        this_box[0] = data_pieces[i];
        FILE *fp;
        char str[80];

        user = this_box->user_id;
        filename = this_box->file_name;

        sprintf(str, "%s%s_%s_%d",MIRROR_I,user,filename,i);
        fp = fopen( str, "wb+");
        fwrite(this_box, sizeof(BOX), 1, fp);
        fwrite(this_box->data, sizeof(char), this_box->data_len, fp);
        fclose(fp);
    }
    /* META WRITING */
    char* python_arg[3] = {"write",user,filename};
    call_python(python_arg);
}

void* list_process(){
    /*TODO: READ meta and give back*/
}

void* get_process(){

}

void* parse_job(JOB* poped_job, void* mes_q){
    uint8_t work     = poped_job->work;
    uint8_t pieces   = poped_job->pieces;
    int     socket_id= poped_job->socket_id;
    BOX* data_pieces = poped_job->data_piece;

    switch(work){
        case JOB_PUT:
            put_process(pieces, data_pieces);
            remove_job(poped_job);
            send_message(mes_q, MESSAGE_TEXT, socket_id, "T", 1);
            printf("Job-put done message enqd(%d)\n", socket_id);
            break;
        case JOB_LIST: list_process(); break;
        case JOB_GET:  get_process(); break;
        default : printf("ERROR, where the work parse processes\n"); break;
    }
}


void* wthr_main(void* job_q){ /*wthr open*/
    void* poped_job;
    void* mes_q = ((JOB_Q*) job_q)->mes_q;
    JOB_Q* this_job_q = (JOB_Q*) job_q;

    while(1){
        if (this_job_q->jobs->count == 0){
            printf("No job... \n");
            sleep(2);
            continue;
        } else {
            sleep(1);
            printf("Yes job... \n");
            poped_job = pop_q(this_job_q);
            printf("pop!\n");
            parse_job(poped_job, mes_q);
            printf("job done!\n");
        }
    }
}
