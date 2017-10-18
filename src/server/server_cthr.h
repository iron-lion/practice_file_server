/*------ SERVER -----*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "job.h"

void printids();
void job_get(char* recv_buffer, int savedSocket, JOB_Q* job_q);
void job_put(char* recv_buffer, int savedSocket, JOB_Q* job_q );
void* open_server();
int main();
