#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "job.h"

/*--- Linked list queue ---*/

void ll_init(list* lptr){
    lptr->count= 0;
    lptr->head = NULL;
    lptr->tail = NULL;
}

void ll_insert(list* lptr, void* jobptr){
    nptr new_nptr=(node*)malloc(sizeof(node));
    new_nptr->jobptr = jobptr;
    new_nptr->next = NULL;
    if (lptr->count==0){
        lptr->head = new_nptr;
        lptr->tail = new_nptr;
    } else {
        lptr->tail->next = new_nptr;
        lptr->tail = new_nptr;
    }
    lptr->count++;
}

void* ll_remove(list* lptr){
    nptr tmp = lptr->head;
    void* pop = tmp->jobptr;
    if(lptr->count!=1){
        lptr->head = tmp->next;
    }
    free(tmp);
    lptr->count--;
    return pop;
}

void ll_print_list(list* lptr){
    nptr tmp=lptr->head;
    printf("List: ");
    while(tmp!=NULL){
        printf("%x ", tmp->jobptr);
        tmp=tmp->next;
    }
    printf("\n Total: %d values\n",lptr->count);
}

/*--- JOB related ---
   head tail useless? */
JOB_Q* create_job_q(){
    JOB_Q* job_q;
    list* jobs=(list*)calloc(0,sizeof(list));
    ll_init(jobs);

    job_q = calloc(0,sizeof(JOB_Q));
    job_q->head = 0;
    job_q->tail = 0;

    job_q->jobs = jobs;
    return job_q;
}

void insert_q(JOB_Q* job_q, void* new_job_pointer){
    list* jobs = job_q->jobs;
    ll_insert(jobs, new_job_pointer);
    job_q->tail += 1;
}

void* pop_q(JOB_Q* job_q){
    void* pop_pointer;
    pop_pointer = ll_remove(job_q->jobs);
    job_q->head += 1;
    return pop_pointer;
}

void remove_job(JOB* job){
    int i;
    if(job->pieces != 0){
        for(i=0;i<job->pieces;i++){
//            printf("::: %d",i);
            BOX tmp_box = job->data_piece[i];
            if(tmp_box.data_len !=0 && tmp_box.data !=NULL){
//                printf(" :::");
                free(tmp_box.data);
//                printf(" :::\n");
            }
        }
    }
    free(job);
}

void remove_job_q(JOB_Q* job_q){
    int head = job_q->head;
    int tail = job_q->tail;
    while (head != tail)
        ll_remove(job_q->jobs);
    free(job_q->jobs);
    free(job_q);
}

/* MESSAGE */
void send_message(JOB_Q* mes_q, int type, int  socket_id, void* text, int text_len){
    MESSAGE* new_message = malloc(sizeof(MESSAGE));
    new_message->type      = type;
    new_message->socket_id = socket_id;
    new_message->text      = text;
    new_message->text_len  = text_len;
    insert_q(mes_q, (void*) new_message);
}

