/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "job.h"
#include "python_caller.h"

void* put_process(uint8_t pieces, BOX* data_pieces){
    FILE *meta_fp;
    char user[8];
    char filename[128];
    int i = 0;
    char piece[16];
    sprintf(piece, "%d",pieces);

    for(i; i<pieces; i++){
        BOX this_box[pieces];
        this_box[0] = data_pieces[i];
        FILE *fp;
        char str[80];

        if(i==0){
            memcpy(user,this_box->user_id,8);
            memcpy(filename,this_box->file_name,128);
        }
        sprintf(str, "%s%s_%s_%d",MIRROR_I,user,filename,i);
        fp = fopen( str, "wb+");
        fwrite(this_box, sizeof(BOX), 1, fp);
        fwrite(this_box->data, sizeof(char), this_box->data_len, fp);
        fclose(fp);
    }
    /* META WRITING */
    printf("%s_%s_%s\n", user, filename, piece);
    char* python_arg[4] = {"write",user,filename,piece};
    call_python(3, python_arg);
}

char* list_process(char* user){
    /* READ meta and give back */
    printf("LETS MALLOC %s\n",user);
    char* text = malloc(BOX_SIZE * 8);
    printf("MALLOC DONE %s\n",user);
    char* python_arg[2] = {"list", user};

    text = call_python(1, python_arg);

    printf("PYTHON CALL DONE %s\n",user);
    return text;
}

void* get_process(char* user_id, char* file_name){
    /*
    BOX new_boxes[pieces];
    for(i; i<pieces; i++){
        BOX new_box = malloc(sizeof(BOX));
        FILE *fp;
        char addr[80];
        char* data;

        sprintf(addr, "%s%s_%s%d", MIRROR_I, user_id, file_name, i);
        fp = fopen( addr, "rb+");
        fread(new_box, sizeof(BOX), 1 fp);
        data = malloc(new_box->data_len);
        fread(data, sizeof(char), new_box->data_len, fp);
        new_box->data = data;
    }*/
}

void* parse_job(JOB* poped_job, void* mes_q){
    char* text_message;
    uint8_t work      = poped_job->work;
    uint8_t pieces    = poped_job->pieces;
    int     socket_id = poped_job->socket_id;
    char*   user      = poped_job->user_id;
    BOX* data_pieces  = poped_job->data_piece;

    switch(work){
        case JOB_PUT:
            put_process(pieces, data_pieces);
            remove_job(poped_job);
            text_message = "T";
            send_message(mes_q, MESSAGE_TEXT, socket_id, text_message, 1);
            printf("Job-put done message enqd(%d)\n", socket_id);
            break;
        case JOB_LIST:
            text_message = list_process(user);
            printf("sending:\n%s\n", text_message);
            send_message(mes_q, MESSAGE_LIST, socket_id, text_message, BOX_SIZE*8);
            remove_job(poped_job);
        case JOB_GET:
            get_process(user, data_pieces->file_name);
            break;
        default : printf("ERROR, where the work parse processes\n"); break;
    }
}


void* wthr_main(void* job_q){ /*wthr open*/
    void*  poped_job;
    void*  mes_q        = ((JOB_Q*) job_q)->mes_q;
    JOB_Q* this_job_q   = (JOB_Q*) job_q;

    while(1){
        if (this_job_q->jobs->count == 0){
            sleep(2);
            continue;
        } else {
            poped_job = pop_q(this_job_q);
            printf("[JOB] poped!\n");
            parse_job(poped_job, mes_q);
            printf("[JOB] done!\n");
        }
    }
}
