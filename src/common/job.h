#include <stdint.h>
#include <string.h>

#define box_size 20
/*--- Linked List | Queue ---*/
typedef struct _node{
    void* jobptr;
    struct _node* next;
} node;
typedef node* nptr;

typedef struct _list{
    int count;
    nptr head, tail;
} list;

void ll_init(list* lptr);
void ll_insert(list* lptr, void* jobptr);
void* ll_remove(list* lptr);
void ll_print_list(list* lptr);

/*  -<<Data Box>>-
 * [00000001]-[13]--------[3]-----[4194304]--[*************]
 *  user_id  file_id  file_offset  data_len       data
 *  32bytes   2bytes      2bytes    4bytes   |data_len|bytes */
typedef struct box{ /*max file size 256GB*/
    char user_id[8];
    char file_name[128];
    uint16_t file_offset;
    uint32_t data_len;
    /* 12bytes */
    char* data;
} BOX;

typedef struct job{
    uint8_t work; /*put get list*/
    uint8_t status;

    BOX* data_piece;
} JOB;

typedef struct job_q{
    uint8_t head, tail;
    list* jobs;
} JOB_Q;

JOB_Q* create_job_q();
void insert_q(JOB_Q* job_q, void* new_job_pointer);
void* pop_q(JOB_Q* job_q);
void remove_job_q(JOB_Q* used_job_q);
