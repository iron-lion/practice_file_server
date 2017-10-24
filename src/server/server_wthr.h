/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "job.h"

void* put_process(uint8_t pieces, BOX* data_pieces);
void* list_process(char* user);
void* get_process(char* user_id, char* file_name, void* mes_q, int socket_id);
void* parse_job(JOB* poped_job, void* mes_q);
void* wthr_main(void* job_q);
