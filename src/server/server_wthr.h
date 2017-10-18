/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "job.h"

void* put_process(uint8_t pieces, BOX* data_pieces);
void* list_process();
void* get_process();
void* parse_job(JOB* poped_job, void* mes_q);
void* wthr_main(void* job_q);
