// Simulation of memory access by process
// Considering locality & transient
// Output: mem_access.txt
// Author: Weichen Xu
// Date: 12/09/2015
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PROCESS_PAGE_NUM	10000
#define SYS_PYHSICAL_MEMORY	10000
#define	PROCESS_TRANSIENT_NUM	10
#define LOCALITY_ATTRIBUTE_1	60
#define LOCALITY_ATTRIBUTE_2	0.1
#define OUTPUT_FILE_NAME	"mem_access_simulation.txt"
// Simulate the process memory access
// simulate locality: like 1, 2, 3 ,4, 3, 2, 4
// simulate PROCESS_TRANSIENT_NUM times transient in the program
// like ...1, 2, 3, 100, 101, 102...
void ranMemAccess(int **mem){
	int base, cur;
	*mem = (int*)malloc(sizeof(int)*(PROCESS_PAGE_NUM+1));
	(*mem)[0] = PROCESS_PAGE_NUM;
	srand(time(NULL));
	// transient 
	for(int i=0; i < PROCESS_TRANSIENT_NUM; i++){
		// locality
		base = rand()%SYS_PYHSICAL_MEMORY;
		for(int j=0; j < PROCESS_PAGE_NUM/PROCESS_TRANSIENT_NUM; j++){
			// (1-LOCALITY_ATTRIBUTE_2) chance inside [base, base+LOCALITY_ATTRIBUTE_1*(1-LOCALITY_ATTRIBUTE_2)]
			// LOCALITY_ATTRIBUTE_2	chance random access memory in the whole sys range
			cur = rand()%LOCALITY_ATTRIBUTE_1;
			// miss the locality
			if(cur > (1-LOCALITY_ATTRIBUTE_2)*LOCALITY_ATTRIBUTE_1){
				cur = rand()%SYS_PYHSICAL_MEMORY;
			}
			else	cur += base;
			(*mem)[i*PROCESS_PAGE_NUM/PROCESS_TRANSIENT_NUM+j+1] = cur;
		}
	}	
}

// Write the simulated memory access to the .txt file
void writeMemFile(int *mem){
	FILE *ofp;
	char *mode = "w+";
	char outputFileName[] = OUTPUT_FILE_NAME;
	ofp = fopen(outputFileName, mode);
	if (ofp == NULL) {
  		fprintf(stderr, "Can't open output file %s!\n", outputFileName);
  		exit(1);
	}
	else	printf("Writing to file:\n");
	for(int i=0; i <= PROCESS_PAGE_NUM; i++){
		fprintf(ofp, "%d\n", mem[i]);
	}
	fclose(ofp);
}
int main(){
	// random memory access
	int *mem = NULL;
	ranMemAccess(&mem);
	writeMemFile(mem);
	if(mem)	free(mem);
}