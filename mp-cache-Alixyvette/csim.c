/******************************************************************************
 *
 * Cache lab header
 *
 * Author: Aaila Ali
 * Email:  aali27@hawk.iit.edu
 * AID:    A20343191
 * Date:   04/11/21
 *
 * By signing above, I pledge on my honor that I neither gave nor received any
 * unauthorized assistance on the code contained in this repository.
 *
 *****************************************************************************/

#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <inttypes.h>

const int m = sizeof(long)*8;
int cacheMisses = 0;
int cacheHits = 0;
int cacheEvicts = 0;
bool verbose = 0;
char* trace;
struct cache* cacheSim; // new*
bool pHit = 0;
bool pMiss = 0;
bool pEvict = 0;
int s;
int E;
int b;

// this is used to keep track of the LRU
// lines contain importabt data such as a valid bit, the tag, and the block
typedef struct cSetLine {
	// the addresses are 64 bit
	struct cSetLine* next;
	struct cSetLine* prev;
	uint64_t tag;
	uint64_t block;		// this is not actually a block of data, it is an offset to the data
				// data size would be 2^b size
				// you would need to allocate the amount of space needed for the data after finding out the size
				// and then index into that saved block using the offset given here
	bool valid;
}cSetLine;

// each of the n sets has a certain number of lines
typedef struct cSet {
	int E;
	cSetLine* lines;
	cSetLine* LRUHead;
	cSetLine* LRUTail;
	//struct cSet* nextSet;
	bool full;
}cSet;

// an n-way associative cache has n lines per set
typedef struct cache {
	int S;
	cSet* sets;
	//bool full;
}cache;

struct cache* init(int s, int E){
	//cache* myCache = (cache*) malloc(sizeof(cache));
	cacheSim = malloc(sizeof(struct cache));
	cacheSim->S = (1<< s);
	//cSet mySet;
	//cSetLine myLine;
	//myCache->sets = (cSet*) malloc(sizeof(cSet)*s);
	cacheSim->sets = calloc(1,pow(2,s)*sizeof(struct cSet));
	cacheSim->sets->E = E;
	//int j = 0;
	//for (i = 0; i < s; i++)
	for (int i = 0; i < cacheSim->S; i++)
	{
		//mySet.lines = (cSetLine*) malloc(sizeof(cSetLine)*E);
		//myCache->sets[i] = mySet;
		//mySet.LRUHead = NULL;
		//mySet.LRUTail = NULL;
		//mySet.full = 0;
		cacheSim->sets[i].lines = calloc(E,sizeof(struct cSetLine));
		
		//cacheSim->sets[i].LRUHead = NULL;
		//cacheSim->sets[i].LRUTail = NULL;
		//for (j = 0; j < E; j++){
//		for (j = 0; j < pow(2,s); j++){
			// calloc sets allocated lines to zero, and pointers to null?
//			cacheSim->sets[i].lines = calloc(E, sizeof(struct c
			//mySet.lines[j] = myLine;
			//myLine.next = NULL;
			//myLine.prev = NULL;
			//myLine.tag = 0;
			//myLine.block = 0;
			//myLine.valid = 0;
//		}
	}
//theCache[setIdx].lines[i].tag = tag;
	return cacheSim;
}

// to avoid memory leaks
void free_cache()
{
	for (int i = 0; i < pow(2,s); i++){
		free(cacheSim->sets[i].lines);
	}
	free(cacheSim->sets);
	free(cacheSim);
	return;
}

void cacheLS( uint64_t addr){//, int size, int sBits, int bBits, int E){
//	printf("Into the sim\n");
	
	// the tag can be found by shifting the address by the number of set and block bits
	uint64_t tag = addr >> (s + b);

	// to find the set, shift the address by the size of the tag and then by the size of the 
	uint64_t setIdx = (addr << (m - (s + b))) >> (m - s);

	// temp variables for convenience
	cSet* theSet = &(cacheSim->sets[setIdx]);
	cSetLine* theLine;

	int i = 0;
	bool hit = 0;
	pHit = 0;
	pMiss = 0;
	pEvict = 0;
	for (i = 0; i < E; i++){
		//checking for a hit
		theLine = &(theSet->lines[i]);
		if (theLine->valid == 1 && theLine->tag == tag) {
			//hit?
			//hit = 1;
			//cacheHits++;
			//update node stuff***************
//			printf("Hit? Where?\n");
			// checking if the set is empty
			// if Head is null, then Tail should be null, but just in case.. lol
			if (theSet->LRUHead != NULL && theSet->LRUTail != NULL){
				if (theLine->tag == theSet->LRUHead->tag){
				//if (theSet->LRUTail != NULL){
					theSet->LRUTail->next = theLine;
				//}
					//theSet->LRUHead = theLine->next;
					theLine->prev = theSet->LRUTail;
					theSet->LRUHead = theLine->next;
					theSet->LRUTail->next->next = NULL;
					theSet->LRUHead->prev = NULL;
					if (theSet->LRUTail->next != NULL){
						theSet->LRUTail = theSet->LRUTail->next;
					}
				//if (theLine->next != NULL){
				//	theLine->next->prev = NULL;
				//}
				//theSet->LRUTail = theLine;
				//}
					hit = 1;
					cacheHits++;
					pHit = 1;
//					printf("Hit! at the head.\n");
					return;

				} else if (theLine->tag != theSet->LRUTail->tag){
					//if (theSet->LRUTail != NULL){
					theSet->LRUTail->next = theLine;
					//}
					//if (theLine->prev != NULL){		// this would only be the case if it was the head, so no need to check
					theLine->prev->next = theLine->next;
					//}
					//if (theLine->next != NULL){		// this would only be the case if it was the tail, so no need to check
					theLine->next->prev = theLine->prev;
					//}
					theLine->prev = theSet->LRUTail;
					theLine->next = NULL;
					theSet->LRUTail = theLine;
					hit = 1;
					cacheHits++;
					pHit = 1;
//					printf("Hit! in the bod.\n");
					return;
				} else if(theLine->tag == theSet->LRUTail->tag){	// just in case, checking
					hit = 1;
					cacheHits++;
					pHit = 1;
//					printf("Hit! at the end.\n");
					return;
				}
			}
			// the end is already the most recently used, so no need to reorder
			//printf("Hit! at the end.\n");
			//return;
		}
	}
	// shouldn't need this but just in case:
	if (!hit){
		cacheMisses++;
		pMiss = 1;
	}
//	printf("Issa miss? Do we evict?\n");
	if (!theSet->full){
	for (i = 0; i < E; i++){
		//miss
		theLine = &(theSet->lines[i]);
		// the set is empty
		if (theSet->LRUHead == NULL){
			theSet->LRUHead = theLine;
			theSet->LRUTail = theLine;
//			printf("Miss! The set was empty!.\n");
			theLine->tag = tag;
			theLine->valid = 1;
			theLine->block = (addr << (m-b)) >> (m-b);
			hit = 1;
			//break;
			return;
		} else if (theLine->valid == 0){
//			printf("valid???\n");
			cacheSim->sets[setIdx].lines[i].tag = tag;
                        cacheSim->sets[setIdx].lines[i].valid = 1;
                        cacheSim->sets[setIdx].lines[i].block = (addr << (m-b)) >> (m-b);
			hit = 1;
//			printf((const char*)theSet->LRUHead);
//			printf("Hi\n");
			//if (theSet->LRUHead == NULL){
			//	// the linked list is empty
			//	theSet->LRUHead = theLine;
			//	theSet->LRUTail = theLine;
			//	printf("Miss! The set was empty.\n");
			//} else {
				// linked list not empty but set not full
			if (theSet->LRUTail != NULL){
				theSet->LRUTail->next = theLine;
			}
			theLine->prev = theSet->LRUTail;
			theSet->LRUTail = theLine;
			theLine->next = NULL;
//			printf("Miss! Not empty, not full.\n");
			//}
			//break;
			return;
		}
	}
	}
	// the set is full and we need to evict
	if (hit != 1){
		theSet->full = 1;
	}
//	printf("Yes we evict. Off with the head!\n");
	cacheEvicts++;
	pEvict = 1;
	theSet->LRUHead->tag = tag;
	theSet->LRUHead->valid = 1;	// redundant
	theSet->LRUHead->block = (addr << (m-b)) >> (m-b);
	hit = 0;
	theSet->LRUTail->next = theSet->LRUHead;
	theSet->LRUHead = theSet->LRUHead->next;
	//theSet->LRUHead->prev = NULL;
	theSet->LRUTail->next->prev = theSet->LRUTail;
	theSet->LRUHead->prev = NULL;
	if (theSet->LRUTail->next == NULL){
		//theLine->next->prev = NULL; // this should never happen
		printf("The impossible.\n");
	}
	theSet->LRUTail->next->next = NULL;
	if (theSet->LRUTail->next != NULL){
		theSet->LRUTail = theSet->LRUTail->next;
	}
	return;
	//************** if size < b or B or something in case you need a second line
}

void helpInfo(){
/*fill*/
	return;
}

int main(int argc, char** argv){
// inputs:
// 	   s, number of sets is 2^s
//	   E, number of lines per set
// 	   b, number of block bits
// 	   t, the valgind file name
// optional inputs:
// 	   h, help flag, it prints usage info
// 	   v, verbose flag
 	// the cache and the file
	FILE* t;
	// for the traces
	unsigned address;
	int size;
	// for making the cache
//	int setBits;
//	int blockBits;
	char c;
	// also for the traces
	char operation;
	// verbosity
	verbose = 0;

	while ((c = getopt(argc, argv, "s:E:b:t:hv")) != EOF) {
		switch (c) {
			case 's':
				s = atoi(optarg);
				//s = pow(2.0,setBits);
				//printf("%d\n",s);
				if(s < 0 || s > m){
					printf("Error: s value must be in [0, word_length].\n");
					exit(-1);
				}
				break;
			case 'E':
				E = atoi(optarg);
				if (E <=0){
					printf("Error: E value must be larger than 0.\n");
					exit(-1);
				}
				//printf("%d\n",E);
				break;
			case 'b':
				b = atoi(optarg);
				//b = pow(2.0,blockBits);
				//printf("%d\n",b);
				if(b < 0 || b > m){
					printf("Error: b valye must be in [0, word_length].\n");
					exit(-1);
				}
				break;
			case 't':
				trace = optarg;
				t = fopen(optarg, "r");
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
				helpInfo();
				exit(0);
			default:
				helpInfo();
				exit(1);
		}
	}
	cacheSim = init(s, E);
	cacheMisses = 0;
	cacheHits = 0;
	cacheEvicts = 0;

	//reading the file and doing stuff for each trace
	while(fscanf(t, "%c %x, %d", &operation, &address, &size) != EOF){
		//printf("%d",size);
		switch(operation){
			case 'I':
				break;
			case 'L':
				if(verbose){printf("L %x,%d", address, size);}
				cacheLS( address);//, size, setBits, blockBits , E);
				if(pMiss){printf(" miss");}
				if(pEvict){printf(" eviction");}
				if(pHit){printf(" hit");}
				printf("\n");
				break;
			case 'S':
				if(verbose){printf("S %x,%d", address, size);}
				cacheLS( address);//, size, setBits, blockBits, E);
				if(pMiss){printf(" miss");}
				if(pEvict){printf(" eviction");}
				if(pHit){printf(" hit");}
				printf("\n");
				break;
			case 'M':
				if(verbose){printf("M %x,%d", address, size);}
				cacheLS( address);//, size, setBits, blockBits, E);
				if(pMiss){printf(" miss");}
				if(pEvict){printf(" eviction");}
				if(pHit){printf(" hit");}
				//printf("\n");
				cacheLS( address);//, size, setBits, blockBits, E);
				if(pMiss){printf(" miss");}
				if(pEvict){printf(" eviction");}
				if(pHit){printf(" hit");}
				printf("\n");
				break;
			default:
				break;
		}
	}
	fclose(t);
//	printf("%d",b);
	printSummary(cacheHits, cacheMisses, cacheEvicts);
	return 0;
}
