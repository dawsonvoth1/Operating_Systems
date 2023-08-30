#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include "queue.h"
#include "util.h"

#define ARRAY_SIZE 8
#define MAX_INPUT_FILES 100
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

// Request struct
typedef struct req {
	// Request log and mutex for writing to it
	FILE* req_log;
	pthread_mutex_t* req_log_lock;	

	// Input files queue and mutex for accessing/changing it					
	queue* inputFiles;	
	pthread_mutex_t* input_lock;	

	// Shared array queue and mutex for accessing it		
	queue* shared_array;			
	pthread_mutex_t* mutex;	

	// Shared conditions for requester/reslover		
	pthread_cond_t* c_req;		
	pthread_cond_t* c_res;		
} request_info;

// Resolver struct
typedef struct res {
	// Resolve log and mutex for writing to it
	FILE* res_log;	
	pthread_mutex_t* res_log_lock;

	// Shared array queue and mutex for accessing it				
	queue* shared_array;					
	pthread_mutex_t* mutex;
	
	// Shared conditions for requester/reslover				
	pthread_cond_t* c_req;			
	pthread_cond_t* c_res;			

	// act as boolean to flag when reqesters are done;
	int* req_done;					
} resolve_info;

void *requester(void* data);

void *resolver(void* data);

#endif
