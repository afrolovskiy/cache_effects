#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/time.h>

#define NPAD 1
#define COUNT 20
#define MAX_POWER 23


struct timeval tv1, tv2, dtv;
struct timezone tz;

void time_start() {
	gettimeofday(&tv1, &tz);
}

long time_stop() {
	gettimeofday(&tv2, &tz);
	dtv.tv_sec= tv2.tv_sec -tv1.tv_sec;
	dtv.tv_usec=tv2.tv_usec-tv1.tv_usec;
	if (dtv.tv_usec < 0) {
		dtv.tv_sec--; 
		dtv.tv_usec += 1000000;
	}
  	return dtv.tv_sec * 1000000 + dtv.tv_usec;
}


char* testing_array;

void init_testing_array(unsigned long size) {
	testing_array= (char*) malloc(size);
}

void release_testing_array() {
	free(testing_array);
}

unsigned long exec_seq_access(unsigned long size) {
	unsigned long i = 0;
	char* p;

	init_testing_array(size);

	time_start();
	for (p = testing_array; i < size; i++, p++) {
		*p; 	
	}
	unsigned long time = time_stop(); 

	release_testing_array();

	return time;
}

void test_sequence_access() {
	FILE* fout = fopen("seq.res", "w");
	printf("test sequence access\n");
	int count = 10;
	unsigned long size = 1024; 
	int pow;
	for (pow = 10; pow <= MAX_POWER; pow++) {
		int c;	
		unsigned long sum_time = 0;
		for (c = 0; c < COUNT; c++) {
			sum_time += exec_seq_access(size);
		}	

		double result_time = (double) sum_time / (double) (COUNT * size * 1000000);
		printf("pow: %d, size: %lu, time access: %f\n", pow, size, result_time); 
		fprintf(fout, "%f\n", result_time);
		
		size *= 2;
	}
	fclose(fout);
}

struct ptr {
	struct ptr* next;
	/*int is_assigned;*/
};

struct ptr* indexes;

void init_rand_indexes(unsigned long amount) {
	unsigned long i, index;

	indexes = (struct ptr*) malloc(sizeof(struct ptr) * amount);
	/*for (i = 0; i < amount; i++) {
		indexes[i].is_assigned = 0;
	}*/

	for (i = 0; i < amount; i++) {
		index = (rand() / (double) RAND_MAX) * amount;
		/*while (indexes[index].is_assigned) {
			index = (rand() / (double) RAND_MAX) * size;
		}*/
		indexes[i].next = &indexes[index];
		/*indexes[index].is_assigned = 1;*/
	}
}

void release_rand_indexes() {
	free(indexes);
}

unsigned long exec_rand_access(unsigned long size) {
	unsigned long i = 0;
	struct ptr* p;
	unsigned long amount = size / sizeof(struct ptr);	

	init_rand_indexes(amount);
	
	time_start();
    	for (p = indexes; i < amount; i++) {
		p = p->next;	
	} 
	unsigned long time = time_stop();

	release_rand_indexes();
	
	return time;
}

void test_rand_access() {
	FILE* fout = fopen("rand.res", "w");
	printf("test random access\n");
	unsigned long size = 1024;
	int pow;
	for (pow = 10; pow <= MAX_POWER; pow++) {
		int c;	
		unsigned long sum_time = 0;
		for (c = 0; c < COUNT; c++) {
			sum_time += exec_rand_access(size);
		}	

		double result_time = (double) sum_time / (double) (COUNT * size * 1000000);
		printf("pow: %d, size: %lu, time access: %f\n", pow, size, result_time); 
		fprintf(fout, "%f\n", result_time);
		
		size *= 2;
	}
	fclose(fout);
}

int main() {
	test_sequence_access();
	test_rand_access();
	return 0;
}
