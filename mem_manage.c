// PFF & VSWS
// Input: file of memory access simulation
// Author: Weichen Xu
// Email: wx431@nyu.edu
// Date: 12/09/2015

/*
 *  I use a test case which considers locality & transient, did not upload the file generating the test case
 *
 *  In PFF
 *	Generally, page fault number decreases when F(reference time) increases
 *  Initially, when F is 1, the page fault number is at maximum, MAX = all mem reference number - continuous access at same address
 *							because page fault can only be avoided when same memory page be accessed in adajcent order
 *             when F is small, the degrowth of page fault is rapid
 *             when F gets bigger, the degrowth of page fault slows down
 *             when F reaches some point, the page fault number is at minimum, MIN = the number of the pages this program occupies
 *	            
 *             when page fault number decreases, the number of frames allocated increases
 *
 *  In VSWS
 *	Generally, page fault number decreases when M increase
 *			   page fault number decreases when L increase
 *             page fault number decreases when Q increase
 *
 *             when page fault number decreases, the number of frames allocated increases
 *
 *  Considering the page fault times in total, PFF is better than VSWS
 *  Considering the frames allocated,  min_frame_allocated_frequency & max_frame_allocated, VSWS is better
 *  	       in other words, VSWS keeps much fewer frames in memory, especially between program transients.
 *  Overall, I think it is a tradeoff because PFF avoid more page faults, which is faster, while VSWS allows more runnable programes
 *             which might avoid CPU wasting
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define F 30
#define M 90
#define Q 20
#define L 150

#define MAX_FILE_NAME_LENGTH 10
#define MAX_MEM_LENGTH	10
#define MAX_MEM_ACCESS_TIME	50000
#define MIN_FRAME_NUMBER 30

typedef struct{
	int address;
	int use_bit;
	bool in_memory;	// true: in residnet set
}Page;

int memAccess(Page *mem_pages, int *mem_acc, int mem_acc_times, int replace_mode);
int loadMemAccessFile(int **mem, int *mem_acc_times);
void shrinkResidentSet(Page *mem_pages, int mem_pages_size);
int PFF(Page *mem_pages, int *mem_pages_size, int page_addr, int cur_reference_time, int *last_reference_time);
int VSWS(Page *mem_pages, int *mem_pages_size, int page_addr, int cur_reference_time, int *last_reference_time, int *elapsed_page_faults);

//int pageReplaceMent(int *mem_acc, int mem_acc_times, int replace_mode);

// get the file name from input
// load the mem access order into array *mem
// Input: **mem: the order of mem access
//        *mem_acc_times: the number of mem access
// Output: how many pages the process occupies, the 1st line of the file
int loadMemAccessFile(int **mem_acc, int *mem_acc_times){
	char fileName[MAX_FILE_NAME_LENGTH];
	char buffer[MAX_MEM_LENGTH];
	char *mode ="r";
	FILE *fp;
	// mem_access_times: the times of memory access by the process
	// pages_num: the number of the pages that process occupies
	int mem_access_times = 0, pages_num;	
	int mem_access[MAX_MEM_ACCESS_TIME];
	
	printf("Enter the file name:");
	scanf("%s", fileName);
	fp = fopen(fileName, mode);
	if(fp == NULL){
		printf("Can not open %s\n", fileName);
		exit(1);
	}
	else printf("Loading mem_acc file\n");

	// get ints line by line
	// load first line to be pages_num
	if(fgets(buffer, MAX_MEM_LENGTH, fp) != NULL){
		sscanf(buffer, "%d\n", &pages_num);
	}
	// the remains to be the mem access sequences
	while(fgets(buffer, MAX_MEM_LENGTH, fp) != NULL){
		sscanf(buffer, "%d\n", &mem_access[mem_access_times]);
		//printf("%d\n", mem_access[mem_access_times]);
		mem_access_times ++;
	}
	// allocate space for *mme and copy data to it
	*mem_acc_times = mem_access_times;
	*mem_acc = (int *)malloc(sizeof(int) * (mem_access_times));
	for(int i=0; i<mem_access_times; i++){
		(*mem_acc)[i] = mem_access[i];
	}
	fclose(fp);
	return pages_num;
}

// shrink resident set size by discarding pages whose use-bit == 0
// set use-bit = 0 with use-bit = 1 before
// *mem_pages: all mem pages of the process, use in_memory to mark resident set instead of dynamic allocation
// mem_pages_size: size of mem_pages
void shrinkResidentSet(Page *mem_pages, int mem_pages_size){
	for(int i=0; i<mem_pages_size; i++){
		// if use-bit is 0, discard
		if(mem_pages[i].use_bit == 0){ 
			mem_pages[i].in_memory = false;
		}
		// otherwise, set from 1 to 0
		else{
			mem_pages[i].use_bit = 0;
		}	
	}
}

// count how many pages are in memory, in other words, the size of resident set
int getResidentSetSize(Page *mem_pages, int mem_pages_size){
	int count = 0;
	for(int i=0; i<mem_pages_size; i++){
		if(mem_pages[i].in_memory)	count++;
	}
	return count;
}
//  simulate the memory access by PFF resident algo
//  restore the page_fault_number in caller
//  return 0: no page fault
//  return 1: page fault

int PFF(Page *mem_pages, int *mem_pages_size, int page_addr, int cur_reference_time, int *most_recent_page_fault_time){
	// the page index in memory set
	int memory_set_index = -1;
	// page fault time
	int pff; 
	// whether page in resident set
	for(int i=0; i<*mem_pages_size; i++){
		if(mem_pages[i].address == page_addr){ 
			memory_set_index = i;
			if(mem_pages[i].in_memory == true) {
				mem_pages[i].use_bit = 1; // set use-bit to 1 when access
				return 0;
			}
			else break;
		}
	}
	// page fault happens
	// if the page is not in the memory set, add it
	if(memory_set_index<0){
		mem_pages[*mem_pages_size].address = page_addr;
		mem_pages[*mem_pages_size].in_memory = true;	// put in resident set
		mem_pages[*mem_pages_size].use_bit = 1;	// set use-bit to 1
		memory_set_index = *mem_pages_size;	
		(*mem_pages_size)++;
	}
	// get the time between two page faults
	pff = cur_reference_time-*most_recent_page_fault_time;
	// set the most recent reference time to current
	*most_recent_page_fault_time = cur_reference_time;
	//printf("pff:%d\n",pff);
	if(pff < F){
		// add to the resident
		// set the use-bit to 1
		mem_pages[memory_set_index].in_memory = true;
		mem_pages[memory_set_index].use_bit = 1;
	}
	else{
		// shrink resident set size by cleaning pages whose use-bit == 0
		// set use-bit = 0 to those pages with use-bit = 1 before
		shrinkResidentSet(mem_pages, *mem_pages_size);
	}
	return 1;
}

// simulate memory access by VSWS
// *mem_page: current memory set, *mem_pages_size: current memory set size
// page_addr: address for memory
// return 1 if page fault occurs
// return 0 otherwise

int VSWS(Page *mem_pages, int *mem_pages_size, int page_addr, int cur_reference_time, int *most_recent_sampling_time, int *elapsed_page_faults){
	// find the index of the page in memory set
	int memory_set_index = -1, page_fault_occur = 1;
	int elapsed_time = cur_reference_time - (*most_recent_sampling_time);
	// whether page in resident set
	for(int i=0; i<*mem_pages_size; i++){
		if(mem_pages[i].address == page_addr){ 
			memory_set_index = i;
			if(mem_pages[i].in_memory == true) {
				page_fault_occur = 0;
				mem_pages[i].use_bit = 1; // set use-bit to 1 when access
			}
			break;
		}
	}

	// if page_fault_occur
	// add elapsed_page_faults by one
	if(page_fault_occur != 0)	(*elapsed_page_faults)++;
	// add this page to the memory set if it not found
	if(memory_set_index<0){
		mem_pages[*mem_pages_size].address = page_addr;
		mem_pages[*mem_pages_size].in_memory = true;	// put in resident set
		mem_pages[*mem_pages_size].use_bit = 1;	// set use-bit to 1
		memory_set_index = *mem_pages_size;	
		(*mem_pages_size)++;
	}

	// if elapsed time reaches L, suspend and scan
	// discard all pages whose use-bit = 0
	// set all remain pages use-bit = 1
	if(elapsed_time >= L){
		shrinkResidentSet(mem_pages, *mem_pages_size);
		*most_recent_sampling_time = cur_reference_time;
		*elapsed_page_faults = 0;
	}
	// if eplapsed_page_faults >= Q and elapsed_reference_time >= M
	// suspend and scan
	else if((*elapsed_page_faults) >= Q && elapsed_time >= M){
		shrinkResidentSet(mem_pages, *mem_pages_size);
		*most_recent_sampling_time = cur_reference_time;
		*elapsed_page_faults = 0;
	}
	return page_fault_occur;
}

// Simulate the process of page fetch & resident set
// *mem_pages: represent the current memory set of the program
//             
// *mem_acc: the sequence of mem access
// mem_acc_times: the number of mem accesss
// replace_mode: 1: for PFF replacement algo
//               2: for VSWS replacement algo
// return the (total) number of page faults
int memAccess(Page *mem_pages, int *mem_acc, int mem_acc_times, int replace_mode){
	int mem_pages_size = 0;
	int page_fault_times = 0;
	// for PFF
	int most_recent_page_fault_time = 0; 
	// for VSWS
	int elapsed_page_faults = 0, most_recent_sampling_time = 0;
	// resident set size statistic, allocated frames
	int max_frame_allocated = 0, min_frame_count = 0, current_frame_allocated;
	float min_frame_frequency = 0.0;
	for(int i=0; i<mem_acc_times; i++){
		// require the i th page according to replacement algo
		if(replace_mode == 1)	{
			// if page fault happpend, page_fault_times ++,  set most_recent_page_fault_time to current
			if(PFF(mem_pages, &mem_pages_size, mem_acc[i], i+1, &most_recent_page_fault_time)){
				page_fault_times++;
			}
		}
		if(replace_mode == 2)	{
			// if page fault happpend, page_fault_times ++
			// if scan the resident set, set most_recent_sampling_time to current
			if(VSWS(mem_pages, &mem_pages_size, mem_acc[i], i+1, &most_recent_sampling_time, &elapsed_page_faults)){
				page_fault_times++;
			}
		}
		// get the number of frames allocated
		current_frame_allocated = getResidentSetSize(mem_pages, mem_pages_size);
		if(current_frame_allocated < MIN_FRAME_NUMBER)	min_frame_count ++;
		max_frame_allocated = max_frame_allocated>current_frame_allocated?max_frame_allocated:current_frame_allocated;
	}
	min_frame_frequency = (float)min_frame_count/(float)mem_acc_times;
	printf("Min frame frequency: %f\n", min_frame_frequency);
	printf("Max frame allocated: %d\n", max_frame_allocated);
	printf("Page fault total: %d\n", page_fault_times);
	return page_fault_times;
}

int main(){
	int *mem_acc = NULL, pages_num, mem_acc_times, page_fault_times = 0;
	Page *mem_pages = NULL;
	pages_num = loadMemAccessFile(&mem_acc, &mem_acc_times);
	// used to test the threshold
	//for(int i=1; i<10; i++){
	//	M = 90;
	//	Q = 40;
	//	L = 150;
		printf("Using PFF\n");
		mem_pages = (Page *)malloc(sizeof(Page) * pages_num);
		page_fault_times = memAccess(mem_pages, mem_acc, mem_acc_times, 1);
		if(mem_pages) free(mem_pages);
		printf("Using VSWS\n");
		mem_pages = (Page *)malloc(sizeof(Page) * pages_num);
		page_fault_times = memAccess(mem_pages, mem_acc, mem_acc_times, 2);
		if(mem_pages) free(mem_pages);
	//}
	//printf("Memory access times: %d\n", mem_acc_times);
	
	if(mem_acc)	free(mem_acc);
	
}