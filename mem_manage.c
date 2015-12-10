// PFF & VSWS
// Input: file of memory access simulation
// Author: Weichen Xu
// Date: 12/09/2015
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define F 20
#define MAX_FILE_NAME_LENGTH 10
#define MAX_MEM_LENGTH	10
#define MAX_MEM_ACCESS_TIME	50000

typedef struct{
	int address;
	int mode;
	bool status;	// true: in residnet set
}Page;

int loadMemAccessFile(int **mem, int *mem_acc_times);
int PFF(Page *mem_pages, int *mem_pages_size, int page_addr, int cur_reference_time, int last_reference_time);


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

//  simulate the memory access by PFF resident algo
//  restore the reference_time & page_fault_number in caller
//  return 0: no page fault
//  return 1: page fault
/*
 *	Generally, page fault number decreases when F(reference time) increases
 *
 *  Initially, when F is 1, the page fault number is at maximum, MAX = all mem reference number - continuous access at same address
 *							because page fault can only be avoided when same memory page be accessed in adajcent order
 *             when F is small, the degrowth of page fault is rapid
 *             when F gets bigger, the degrowth of page fault slows down
 *             when F reaches some point, the page fault number is at minimum, MIN = the size of working set
 *                          in other words, the number of the pages this program occupies
*/
int PFF(Page *mem_pages, int *mem_pages_size, int page_addr, int cur_reference_time, int last_reference_time){
	// the page index in working set
	int working_set_index = -1;
	// page fault time
	int pff; 
	// whether page in resident set
	for(int i=0; i<*mem_pages_size; i++){
		if(mem_pages[i].address == page_addr){ 
			working_set_index = i;
			if(mem_pages[i].status == true) {
				mem_pages[i].mode = 1; // set use-bit to 1 when access
				return 0;
			}
			else break;
		}
	}
	// page fault happens
	// if the page is not in the working set, add it
	if(working_set_index<0){
		mem_pages[*mem_pages_size].address = page_addr;
		mem_pages[*mem_pages_size].status = true;	// put in resident set
		mem_pages[*mem_pages_size].mode = 1;	// set use-bit to 1
		working_set_index = *mem_pages_size;	
		(*mem_pages_size)++;
	}
	pff = cur_reference_time-last_reference_time;
	//printf("pff:%d\n",pff);
	if(pff < F){
		// add to the resident
		// set the use-bit to 1
		mem_pages[working_set_index].status = true;
		mem_pages[working_set_index].mode = 1;
	}
	else{
		// shrink resident set size by cleaning pages whose use-bit == 0
		// set use-bit = 0 to those pages with use-bit = 1 before
		for(int i=0; i<*mem_pages_size; i++){
			if(mem_pages[i].mode == 0){ 
				mem_pages[i].status = false;
			}
			else{
				mem_pages[i].mode = 0;
			}	
		}
	}
	return 1;
}
// Simulate the process of page fetch & replacement
// *mem_pages: represent the page status in memory
//             
// *mem_acc: the sequence of mem access
// mem_acc_times: the number of mem accesss
// replace_mode: 1: for PFF replacement algo
//               2: for VSWS replacement algo
// return the (total) number of page faults
int memAccess(Page *mem_pages, int *mem_acc, int mem_acc_times, int replace_mode){
	// Initially, resident size = 0;
	int mem_pages_size = 0, page_fault_times = 0, most_recent_page_fault_time = 0; 
	for(int i=0; i<mem_acc_times; i++){
		// require the i th page according to replacement algo
		if(replace_mode == 1)	{
			// if page fault happpend, page_fault_times ++,  set most_recent_page_fault_time to current
			if(PFF(mem_pages, &mem_pages_size, mem_acc[i], i+1, most_recent_page_fault_time)){
				most_recent_page_fault_time = i+1;
				page_fault_times++;
			}
		}
	}
	return page_fault_times;
}

int main(){
	int *mem_acc = NULL, pages_num, mem_acc_times, page_fault_times = 0;
	Page *mem_pages = NULL;
	pages_num = loadMemAccessFile(&mem_acc, &mem_acc_times);
	mem_pages = (Page *)malloc(sizeof(Page) * pages_num);
	page_fault_times = memAccess(mem_pages, mem_acc, mem_acc_times, 1);
	printf("Memory access times: %d\n", mem_acc_times);
	printf("Page fault times: %d\n", page_fault_times);
	if(mem_acc)	free(mem_acc);
	if(mem_pages) free(mem_pages);
}