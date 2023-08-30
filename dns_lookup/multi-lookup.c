#include "multi-lookup.h"

void *requester(void* data) {
	request_info* req = (request_info*) data;
	pthread_t tid = pthread_self();
	int thread_id = (int) tid;
	int file_count  = 0;
    FILE* input_file;
	while(1) {
		// lock input file queue while accessing
		pthread_mutex_lock(req->input_lock);

		// if the input file queue is not empty, dequeue and work through that file
		if(!isEmpty(req->inputFiles)) {
			char* curr_input_file = dequeue(req->inputFiles);
			file_count++;
			pthread_mutex_unlock(req->input_lock);

			// open current file from input file queue
			input_file = fopen(curr_input_file, "r");
			char hostName[MAX_NAME_LENGTH];

			// read each hostName individually by line
			while(fgets(hostName, sizeof(hostName), input_file)) {
				// lock shared array while accessing
				pthread_mutex_lock(req->mutex);
				// wait if shared array is full
				while(isFull(req->shared_array)) {
					pthread_cond_wait(req->c_req, req->mutex);
				}

				// copy the hostname and enqueue to the shared array
				char* host = strdup(strtok(hostName, "\n"));
				enqueue(req->shared_array, host);

				// unlock mutex and signal resolver
				pthread_mutex_unlock(req->mutex);
				pthread_cond_signal(req->c_res);
			}
			fclose(input_file);
			free(curr_input_file);
		} 
		else {
			pthread_mutex_unlock(req->input_lock);
			break;
		}
	}
	// write to request log
	pthread_mutex_lock(req->req_log_lock);
	fprintf(req->req_log, "thread %d serviced %d files\n", thread_id, file_count);
	pthread_mutex_unlock(req->req_log_lock);
	return 0;
}

void *resolver(void* data) {
	resolve_info* res = (resolve_info*) data;
	pthread_t tid = pthread_self();
	int thread_id = (int) tid;
	int hostname_count = 0;
	char *ip = malloc(MAX_IP_LENGTH);
	while(1) {
		// lock shared array while accessing
		pthread_mutex_lock(res->mutex);

		// while requesters are not done or shared array is not empty
		if(*(res->req_done) == 0 || !isEmpty(res->shared_array)) {
			// wait if shared array is empty
			while(isEmpty(res->shared_array)) {
				pthread_cond_wait(res->c_res, res->mutex);
			}
			char* hostname = dequeue(res->shared_array);
			pthread_mutex_unlock(res->mutex);

			// signal requester condition
			pthread_cond_signal(res->c_req);

			// write to resolver log if ip can not be found
			if(dnslookup(hostname, ip, MAX_IP_LENGTH) == UTIL_FAILURE) {
				pthread_mutex_lock(res->res_log_lock);
				fprintf(res->res_log, "%s, NOT_RESOLVED \n", hostname);
				pthread_mutex_unlock(res->res_log_lock);
			} 
			else {
				// write to resoler log if ip can be found
				pthread_mutex_lock(res->res_log_lock);
				fprintf(res->res_log, "%s, %s\n", hostname, ip);
				pthread_mutex_unlock(res->res_log_lock);
			}
			hostname_count ++;
			free(hostname);
		} 
		else {
			pthread_mutex_unlock(res->mutex);
			break;
		}
	}
	free(ip);
	pthread_mutex_lock(res->res_log_lock);
	fprintf(stdout, "thread %d resolved %d hostnames\n", thread_id, hostname_count);
	pthread_mutex_unlock(res->res_log_lock);
	return 0;
}


int main(int argc, char* argv[]) {

	// start program timer
	struct timeval start, stop;
	gettimeofday(&start, NULL);

	// multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ...]
	// check arguments
	if(argc <= 5) {
		fprintf(stdout, "Missing input arguments. Please include multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ...].\n");
		exit(1);
	} else if(argc > 5 + MAX_INPUT_FILES) {
		fprintf(stderr, "ERROR: too many input files\n");
		exit(1);	
	}
	

	// check num threads
	int num_requester = atoi(argv[1]);
	if(num_requester < 1) {
		fprintf(stderr, "ERROR: need at least 1 requester thread\n");
		exit(1);
	} else if (num_requester > MAX_REQUESTER_THREADS) {
		fprintf(stderr, "ERROR: max number of requester threads is %d\n", MAX_REQUESTER_THREADS);
		exit(1);
	}
	int num_resolver = atoi(argv[2]);
	if(num_resolver < 1) {
		fprintf(stderr, "ERROR: need at least 1 resolver thread\n");
		exit(1);
	} else if (num_resolver > MAX_RESOLVER_THREADS) {
		fprintf(stderr, "ERROR: max number of resolver threads is %d\n", MAX_RESOLVER_THREADS);
		exit(1);
	}

	// add input files to queue
	queue input_files;
	init_queue(&input_files, MAX_INPUT_FILES);
	for(int i = 5; i < argc; i++) {
		if(access(argv[i], F_OK) != 0) {
			fprintf(stderr, "invalid file %s\n", argv[i]);
		} else {
			char* data = strdup(argv[i]);
			enqueue(&input_files, data);
		}
	}
	if(isEmpty(&input_files)) {
		fprintf(stderr, "ERROR: no input files\n");
		exit(1);
	}

	// Initialize mutexs and conditions
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t c_req = PTHREAD_COND_INITIALIZER;
	pthread_cond_t c_res = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t input_lock = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t req_log_lock = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t res_log_lock = PTHREAD_MUTEX_INITIALIZER;


	// Initialize shared array
	int req_done = 0;
	queue sharedArr;
	int queue_size = 50;
	init_queue(&sharedArr, queue_size);

	// Initialize structs for shared array data
	request_info req;
	req.req_log = fopen(argv[3], "w");
	req.inputFiles = &input_files;
	req.shared_array = &sharedArr;
	req.mutex = &mutex;
	req.c_req = &c_req;
	req.c_res = &c_res;
	req.input_lock = &input_lock;
	req.req_log_lock = &req_log_lock;

	resolve_info res;
	res.res_log = fopen(argv[4], "w");
	res.shared_array = &sharedArr;
	res.mutex = &mutex;
	res.c_req = &c_req;
	res.c_res = &c_res;
	res.res_log_lock = &res_log_lock;
	res.req_done = &req_done;

	pthread_t request_tid[num_requester];
	pthread_t resolve_tid[num_resolver];

	// threads
	for(int i = 0; i < num_requester; i++) {
		pthread_create(&request_tid[i], NULL, requester, &req);
	}
	for(int i = 0; i < num_resolver; i++) {
		pthread_create(&resolve_tid[i], NULL, resolver, &res);
	}
	

	for(int i = 0; i < num_requester; i++) {
	    pthread_join(request_tid[i], NULL);
	}
	pthread_mutex_lock(&mutex);
    req_done = 1;
    pthread_mutex_unlock(&mutex);
    
    for(int i = 0; i < num_resolver; i++) {
	    pthread_join(resolve_tid[i], NULL);
    }
    fclose(req.req_log);
    fclose(res.res_log);
    free_queue(&input_files);
    free_queue(&sharedArr);

    gettimeofday(&stop, NULL);
    printf("./multi-lookup total time: %ld seconds\n", stop.tv_sec-start.tv_sec);
	return 0;
}
