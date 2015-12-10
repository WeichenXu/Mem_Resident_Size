// PFF & VSWS
// Input: file of memory access simulation
// Author: Weichen Xu
// Date: 12/09/2015
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define F 0.1
#define MAX_FILE_NAME_LENGTH 100
#define MAX_MEM_LENGTH	10
#define MAX_MEM_ACCESS_TIME	50000
// get the file name from input
// load the mem access order into array *mem
// Input: **mem: the order of mem access
//        *mem_acc_times: the number of mem access
// Output: how many pages the process occupies, the 1st line of the file
int loadMemAccessFile(int **mem, int *mem_acc_times){
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
	*mem = (int *)malloc(sizeof(int) * (mem_access_times));
	for(int i=0; i<mem_access_times; i++){
		(*mem)[i] = mem_access[i];
	}
	return pages_num;
}
int main(){
	int *mem = NULL, pages_num, mem_acc_times;
	pages_num = loadMemAccessFile(&mem, &mem_acc_times);
	for(int i=0; i<mem_acc_times; i++){
		printf("mem_access:%d\t address: %d\n", i, mem[i]);
	}
	if(mem)	free(mem);
}