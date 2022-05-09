/******************************************************************************
 * 
 * Malloc lab header
 * 
 * Author: Aaila Ali
 * Email:  aali27@hawk.iit.edu
 * AID:    A20343191
 * Date:   05/02/21
 * 
 * By signing above, I pledge on my honor that I neither gave nor received any
 * unauthorized assistance on the code contained in this repository.
 * 
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "mm.h"
#include "memlib.h"

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


// additional macros
//#define WORDSZ 4
//#define DOUBSZ 8
//#define INITSZ 64
//#define BLKSZ 4096
//#define INITSZ 8
//#define CHUNKSZ 16
#define LISTLIM 16
//#define OVERHEAD 24
//#define MAX(x,y) ((x) > (y) ? (x) : (y))
//#define MIN(x,y) ((x) < (y) ? (x) : (y))

//#define SZALLOC(size, alloc) ((size) | (alloc))
//#define INSERT(p, val) (*(size_t*)(p) = (val))
//#define INSERTT(p, val) (*(size_t*)(p) = (val) | GETTAG(p))

// a bunch of GET macros
//#define GET(p) (*(size_t*)(p))
//#define GETSZ(p) (GET(p) & ~0x7)	// does opposite of SZALLOC
//#define GETALLOC(p) (GET(p) & 0x1)	// does opposite of SZALLOC
//#define GETTAG(p) (GET(p) & 0x2)

//#define SETPTR(p, ptr) (*(size_t*)(p) = (size_t)(ptr))

// dealing with block header and footers
#define HEADP(p, toP) ( ((blkFoot*)((char*)p + PCOMP(p) - ALIGN(sizeof(blkFoot))))->head = toP)

//#define FOOTP(p) ((void*)(p) + GETSZ(HEADP(p) - DOUBSZ))
#define NEXTP(p, toP) (p->next = toP)
#define PREVP(p, toP) (p->prev = toP)
#define ANEXTP(p, toP) (p.next = toP)
#define APREVP(p, toP) (p.prev = toP)
#define PCOMP(p) (p->size & ~1)
//#define NEXTPFREE(p) (*(void**)(p) + DOUBSZ)
//#define PREVPFREE(p) (*(void**)(p))
// might need more
//#define NEXTA(p) (*(char**)(NEXTPFREE(p)))
//#define PREVA(p) (*(char**)(p))
#define GETSZ(p, newSz) (p->size = newSz)
#define AGETSZ(p, newSz) (p.size = newSz)
#define PDELT(p) {NEXTP(p->prev, p->next); PREVP(p->next, p->prev); }

typedef struct header blkHead;
typedef struct footer blkFoot;

struct header {
	size_t size;
	blkHead* next;
	blkHead* prev;
};

struct footer {
	blkHead* head;
};


// function prototypes
//static void* coalesce(void* p);
//static void insertCoal(void* p);
static blkHead* findBLK(size_t sz);
//static void insertBLK(void* p, size_t sz);
//static void removeFBLK(void* p);
//static void* heapFix(size_t numW);
//static void toSplit(void* p, size_t sz);
static int getLen(size_t sz);

// pointers representing memory list(s)
//static char* freeList = 0;
//static char* blkList = 0;
//void* myList[LISTLIM];
struct header* myList;

int i, *listLen;

// static functions

static int getLen(size_t sz){
	for(i = 0; i < LISTLIM; i++){
		if (sz <= listLen[i]) { return i; } }
	return LISTLIM - 1;
}

/*
static void* coalesce(void* p) {
	size_t prevBlk = 0;
	size_t nextBlk = 0;
//	if (p > mem_heap_lo()) {
//		prevBlk = ((*(size_t*)(p))&(~0x7));
	prevBlk = GETALLOC( FOOTP( PREVP(p) ) ) || PREVP(p) == p;
//		if (GETTAG( HEADP( PREVP(p)))) { prevBlk = 1; }
//	}
//	if (p < mem_heap_hi()) {
	nextBlk = GETALLOC( HEADP( NEXTP(p) ) );
//	}
	size_t currSz = GETSZ( HEADP(p) );
	
	//if (GETTAG( HEADP( PREVP(p)))) { prevBlk = 1; }

//	if (prevBlk && nextBlk) {			// case 1
//		return p;
//	} else 
	if (prevBlk && !nextBlk) {		// case 2
//		removeFBLK(p);
//		removeFBLK( NEXTP(p) );
		currSz += GETSZ( HEADP( NEXTP(p) ) );
		removeFBLK( NEXTP(p) );
		INSERT( HEADP(p), SZALLOC(currSz, 0) );
		INSERT( FOOTP(p), SZALLOC(currSz, 0) );
	} else if (nextBlk && !prevBlk) {		// case 3
//		removeFBLK(p);
//		removeFBLK( PREVP(p) );
		currSz += GETSZ( HEADP( PREVP(p) ) );
		p = PREVP(p);
		removeFBLK(p);
		INSERT( HEADP(p), SZALLOC(currSz, 0) );
		INSERT( FOOTP(p), SZALLOC(currSz, 0) );//PREVP(p) ), SZALLOC(currSz, 0) );
//		p = PREVP(p);
	} else if (!prevBlk && !nextBlk) {		// case 4
//		removeFBLK(p);
//		removeFBLK( PREVP(p) );
//		removeFBLK( NEXTP(p) );
		currSz += GETSZ( HEADP( NEXTP(p) ) ) + GETSZ( HEADP( PREVP(p) ) );
		removeFBLK( PREVP(p) );
		removeFBLK( NEXTP(p) );
		p = PREVP(p);
		INSERT( HEADP( PREVP(p) ), SZALLOC(currSz, 0) );
		INSERT( FOOTP( NEXTP(p) ), SZALLOC(currSz, 0) );
//		p = PREVP(p);
	}
	insertCoal(p);
	return p;
}

static void insertCoal(void* p) {
	NEXTPFREE(p) = freeList;
	PREVPFREEd *currP;
    for (i = getBin(size), currP = myList[i].next; i < LISTLIM; i++, currP = myList[i].next) {
        while ((currP != &myList[i]) && (currP->size < size + SIZE_T_SIZE + ALIGN(sizeof(blkFoot))))
            currP = currP->next;
        if (currP != &myList[i]) return currP;
    }
    return NULL;(freeList) = p;
	PREVPFREE(p) = NULL;
	freeList = p;
//	insertBLK(p, currSz);
//	return p;	// the coalesced block
}
*/
static blkHead* findBLK(size_t sz) {
	blkHead *currP;
	for (i = getLen(sz), currP = myList[i].next; i < LISTLIM; i++, currP = myList[i].next) {
		while ((currP != &myList[i]) && (currP->size < sz + SIZE_T_SIZE + ALIGN(sizeof(blkFoot)))) {
			currP = currP->next;
		}
		if (currP != &myList[i]) return currP;
	}
	return NULL;
}
/*
static void insertBLK(void* p, size_t sz) {
	int tempLen = 0;
	void* theP = p;
	void* tempP = NULL;
	while ((tempLen < LISTLIM - 1) && (sz > 1)) {
		sz >>= 1;
		tempLen++;
	}
	theP = myList[tempLen];
	while ((theP != NULL) && (sz > GETSZ( HEADP(theP)))) {
		tempP = theP;
		theP = PREVA(theP);
	}
	if (theP != NULL) {
		if (tempP != NULL) {
			SETPTR( PREVPFREE(p), theP );
			SETPTR( NEXTPFREE(theP), p );
			SETPTR( NEXTPFREE(p), tempP );
			SETPTR( PREVPFREE(tempP), p );
		} else {
			SETPTR( PREVPFREE(p), theP );
			SETPTR( NEXTPFREE(theP), p );
			SETPTR( NEXTPFREE(p), NULL);
			myList[tempLen] = p;
		}
	} else {
		if (tempP != NULL) {
			SETPTR( PREVPFREE(p), NULL );
			SETPTR( NEXTPFREE(p), tempP );
			SETPTR( PREVPFREE(tempP), p );
		} else {
			SETPTR( PREVPFREE(p), NULL);
			SETPTR( NEXTPFREE(p), NULL);
			myList[tempLen] = p;
		}
	}
	size_t tempSz = GETSZ( HEADP(p) );
	size_t diffSz = tempSz - sz;
	if ( diffSz >= BLKSZ) {
		INSERT( HEADP(p), SZALLOC(sz, 1) );
		INSERT( FOOTP(p), SZALLOC(sz, 1) );
		removeFBLK(p);
		INSERT( HEADP(p), SZALLOC(diffSz, 1) );
		INSERT( FOOTP(p), SZALLOC(diffSz, 1) );
		coalesce(p);	
	} else {
		INSERT( HEADP(p), SZALLOC(tempSz, 1) );
		INSERT( FOOTP(p), SZALLOC(tempSz, 1) );
		removeFBLK(p);
	}
	return;
}*/

/*
static void removeFBLK(void* p) {
	int tempLen = 0;
	size_t sz = GETSZ( HEADP(p) );
	while ((tempLen < LISTLIM - 1) && (sz > 1)){
		sz >>= 1;
		tempLen++;
	}
	//if (p) {
//	if ( PREVA(p) != NULL) {
	if ( PREVPFREE(p) ){
		if ( NEXTA(p) != NULL) {
			SETPTR( NEXTPFREE( PREVA(p)), NEXTA(p));
			SETPTR( PREVPFREE( NEXTA(p)), PREVP(p));
			//NEXTPFREE( PREVPFREE(p) ) = NEXTPFREE(p);
		} else { //freeList = NEXTPFREE(p); }
			SETPTR( NEXTPFREE( PREVA(p)), NULL);
			myList[tempLen] = PREVA(p);
		}
		//if ( NEXTPFREE(p) != NULL ) {
		NEXTPFREE( PREVPFREE(p) ) = NEXTPFREE(p);
		//}
	} else {
		freeList = NEXTPFREE(p);
		if ( NEXTA(p) != NULL) {
			SETPTR( PREVPFREE( NEXTA(p)), NULL);
		} else {
			myList[tempLen] = NULL;
		}
	}
	PREVPFREE( NEXTPFREE(p) ) = PREVPFREE(p);
	return;
}

static void* heapFix(size_t numW) {
	char* p;
	size_t sz = (numW % 2) ? ((numW + 1) * WORDSZ) : (numW * WORDSZ);
//	size_t sz = ALIGN(numW);
	if (sz < OVERHEAD) { sz = OVERHEAD; }

	if ((long)(p = mem_sbrk(sz)) == -1) { return NULL; }

	INSERT( HEADP(p), SZALLOC(sz, 0) );
	INSERT( FOOTP(p), SZALLOC(sz, 0) );
	INSERT( HEADP( NEXTP(p) ), SZALLOC(0, 1) );
//	insertBLK(p, sz);
	return coalesce(p);
}

static void toSplit(void* p, size_t sz) {
	size_t pSz = GETSZ( HEADP(p) );
	size_t diffSz = pSz - sz;
//	removeFBLK(p);

	if (diffSz >= OVERHEAD) {	// no split, else splits
//		INSERTT( HEADP(p), SZALLOC(pSz, 1) );
//		INSERTT( FOOTP(p), SZALLOC(pSz, 1) );
//	} else if (sz >= 100) {		// try diff values
		INSERT( HEADP(p), SZALLOC(sz, 1) );
		INSERT( FOOTP(p), SZALLOC(sz, 1) );
		removeFBLK(p);
		INSERT( HEADP(p), SZALLOC(diffSz, 0) );
		INSERT( FOOTP(p), SZALLOC(diffSz, 0) );
//		insertBLK(p, diffSz);
//		return NEXTP(p);
		coalesce(p);
	} else {
		INSERT( HEADP(p), SZALLOC(pSz, 1) );
		INSERT( FOOTP(p), SZALLOC(pSz, 1) );
//		INSERT( HEADP( NEXTP(p) ), SZALLOC(diffSz, 0) );
//		INSERT( FOOTP( NEXTP(p) ), SZALLOC(diffSz, 0) );
//		insertBLK( NEXTP(p), diffSz);
		removeFBLK(p);
	}
//	return p;
}*/

// lab functions
int mm_init(void) {
	//int tempLen;
	//char* theHead;
	//if((blkList = mem_sbrk(INITSZ + BLKSZ)) == (void*)-1){ return -1; }
	myList = mem_sbrk(LISTLIM * ALIGN(sizeof(blkHead)));//) == NULL) { return -1; }
	listLen = mem_sbrk(LISTLIM * sizeof(int));

	listLen[0] = 8;

	for (i = 1; i < LISTLIM; i++){
		listLen[i] = listLen[i-1] * 2;
	}

	blkHead* tempPh = mem_sbrk((ALIGN(sizeof(blkHead)) + ALIGN(sizeof(blkFoot))));
	blkFoot* tempPf = (blkFoot*) ((char*) tempPh + ALIGN(sizeof(blkHead)));
	NEXTP(tempPh, PREVP(tempPh, tempPh));

	GETSZ(tempPh, ((ALIGN(sizeof(blkHead)) + ALIGN(sizeof(blkFoot))) + 1));
	tempPf->head = tempPh;

//	if ((long)(theHead = mem_sbrk(4 * WORDSZ)) == -1) { return -1; }

//	INSERT(blkList, 0);		// might not need second argument here
//	INSERT(blkList + WORDSZ, SZALLOC(OVERHEAD, 1));
//	INSERT(blkList + (WORDSZ*2), 0);
//	INSERT(blkList + (WORDSZ*3), 0);
//	INSERT(blkList + OVERHEAD, SZALLOC(OVERHEAD, 0);
//	INSERT(blkList + OVERHEAD + WORDSZ, SZALLOC(0, 1));
	
//	freeList = blkList + DOUBSZ;

//	if (heapFix(CHUNKSZ/WORDSZ) == NULL) { return -1; }

/*	for (i = 0; i < LISTLIM; i++){
		myList[i].next = (myList[i].prev = &myList[i]);
		myList[i].size = (ALIGN(sizeof(blkHead)) + ALIGN(sizeof(blkFoot)) + 1 );
	}*/
	for (i = 0; i < LISTLIM; i++) {
		ANEXTP(myList[i], APREVP(myList[i], &myList[i]));
		AGETSZ(myList[i], (ALIGN(sizeof(blkHead)) + ALIGN(sizeof(blkFoot))) + 1);
	}	
	return 0;
}

void *mm_malloc(size_t size) {
//	int tempLen = 0;
//	size_t sz;
	size_t newSz;
//	size_t moreSz;
//	size_t tempSz = 0;

	blkHead* p;
//	blkHead* split;
//	blkHead* temp;
//	blkFoot* tempF;
//	int i, *listLen;
//	if (size <= 0) { return NULL; }
	newSz = ALIGN(size + SIZE_T_SIZE + ALIGN(sizeof(blkFoot)));
	if ((p = findBLK(newSz))) {
		if (PCOMP(p) - newSz >= 7 + (ALIGN(sizeof(blkHead)) + ALIGN(sizeof(blkFoot)))) {
			size_t tempSz = p->size;
			GETSZ(p, newSz);
			HEADP(p, p);

			blkHead* tempX = (blkHead*)((char*) p + PCOMP(p));

			GETSZ(tempX, tempSz - p->size);
			HEADP(tempX, tempX);
			i = getLen(tempX->size - SIZE_T_SIZE - ALIGN(sizeof(blkFoot)));
			NEXTP(tempX, myList[i].next);
			myList[i].next = PREVP(tempX->next, tempX);
			PREVP(tempX, &myList[i]);
		}
//		toSplit(p, newSz);
		//return p;
		PDELT(p);
		p->size |= 1;

	} else {
		blkHead* tempY = (((blkFoot*)((char*) mem_heap_hi() + 1 - ALIGN(sizeof(blkFoot))))->head);
		if (!(tempY->size & 1)) {
			mem_sbrk(newSz - tempY->size);
			GETSZ(tempY, newSz | 1);
			HEADP(tempY, tempY);
			PDELT(tempY);
			p = tempY;
		} else {
			if (!(p = mem_sbrk(newSz))) { return NULL; }
			else {
				GETSZ(p, newSz | 1);
				blkFoot* tempZ = (blkFoot*)((char*) p + PCOMP(p) - ALIGN(sizeof(blkFoot)));
				tempZ->head = p;
			}
		}
	}

	//tempSz = MAX(newSz, CHUNKSZ);
/*	if ((p = heapFix(tempSz/WORDSZ)) == NULL){ return NULL; }
	
	toSplit(p, newSz);
	return(p);*/
//  int blk_size = ALIGN(size + SIZE_T_SIZE);
//  size_t *header = mem_sbrk(blk_size);
/*	if (size < DOUBSZ) {
		sz = 2 * DOUBSZ;
	} else {
		sz = ALIGN( size + DOUBSZ);
	}
*//*	tempSz = sz;

  if (header == (void *)-1) {
    return NULL;
  } else {
    *header = blk_size | 1;
    return (char *)header + SIZE_T_SIZE;
  }*/
/*	while (tempLen < LISTLIM) {
		if ((tempLen == LISTLIM -1) || ((tempSz <= 1) && (myList[tempLen] != NULL))) {
			//p = PREVA(p);
			p = myList[tempLen];
			while ((p != NULL) && ((sz > GETSZ( HEADP(p) ) ) || (GETTAG( HEADP(p) )))) {
				p = PREVA(p);
			}
			if (p != NULL) { break; }
		}
		tempSz >>= 1;
		tempLen++;
	}
	if (p == NULL) {
		moreSz = MAX(sz, BLKSZ);
		if ((p = heapFix(moreSz)) == NULL) { return NULL; }
	}
	p = toSplit(p, sz);
	return p;*/
	return (void*)((char*)p + sizeof(size_t));
}

void mm_free(void *ptr) {
//  size_t *header = ptr - SIZE_T_SIZE;
//  *header &= ~1L;
	blkHead* currP = (blkHead*)((char*) ptr - SIZE_T_SIZE);
	blkHead* prevP = ((blkFoot*)((char*) currP - ALIGN(sizeof(blkFoot))))->head;
	blkHead* tempN;
	blkHead* tempF;
	GETSZ(currP, PCOMP(currP));
	//p->size
	if (!((prevP->size) & 1)) {
		PDELT(prevP);
		prevP->size += currP->size;
		HEADP(currP, prevP);
		currP = prevP;
	}

	tempN = (blkHead *) ((char *) currP + PCOMP(currP));
	if (((void*)((char*) currP + currP->size) <= mem_heap_hi()) && !((tempN->size) & 1)) {
		currP->size += tempN->size;
		HEADP(currP, currP);
		PDELT(tempN);
    	}

	tempF = &myList[getLen(currP->size - SIZE_T_SIZE - ALIGN(sizeof(blkFoot)))];
	PREVP(currP, tempF);
	NEXTP(currP, tempF->next);
	PREVP(tempF->next, currP);
	NEXTP(tempF, currP);
/*
//	GET( HEADP( NEXTP(ptr) ) ) &= ~0x2;		// removing tag
	INSERT( HEADP(ptr), SZALLOC(sz, 0) );
	INSERT( FOOTP(ptr), SZALLOC(sz, 0) );
//	insertBLK(ptr, sz);
	coalesce(ptr);
*/
}

void *mm_realloc(void *ptr, size_t size) {
//  void *oldptr = ptr;
	blkHead* currP = (blkHead*)((char*) ptr - sizeof(size_t));
        blkHead* nextP = (blkHead*)((char*) currP + (currP->size &= ~1));

//	blkHead* newptr = (blkHead*)((char*)ptr - sizeof(size_t));
//	size_t copySize = size;
	size_t newSz = ALIGN(size + SIZE_T_SIZE + ALIGN(sizeof(blkFoot)));
//	size_t oldSz;
//	int diffSz;
//	int moreSz;
//	int buff;

	if (PCOMP(currP) > newSz) {
		return ptr;
	}

/*	if (size <= 0) {
		mm_free(ptr); // ALWAYS NEED TO FREE!!!
		return NULL;
	}
//  newptr = mm_malloc(size);
	if (ptr == NULL) {
		return mm_malloc(size);
	}
//  copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
	oldSz = GETSZ( HEADP(ptr) );

	if (oldSz == newSz) {
		return ptr;
	}
	if (newSz <= oldSz) {
		size = newSz;
		diffSz = oldSz - size;
		if (diffSz <= OVERHEAD) { return ptr; }
		
		INSERT( HEADP(ptr), SZALLOC(size, 1) );
		INSERT( FOOTP(ptr), SZALLOC(size, 1) );
		INSERT( HEADP( NEXTP(ptr) ), SZALLOC(diffSz, 1) );
		mm_free(NEXTP(ptr));
		return ptr;
	}
	newptr = mm_malloc(size);
	if (!newptr) { return 0; }
	
	if (size < oldSz) { oldSz = size; }

	memcpy(newptr, ptr, oldSz);
//	return newptr;
*//*	if (copySize <= DOUBSZ){
		copySize = 2 * DOUBSZ;
//    copySize = size;
	} else {
		copySize = ALIGN(size + DOUBSZ);
	}
	copySize += 128;
	buff = GETSZ( HEADP(ptr) ) - copySize;
	if (buff < 0) {
		if (!GETALLOC( HEADP( NEXTP(ptr) ) ) || !GETSZ( HEADP( NEXTP(ptr) ) ) ) {
			diffSz = GETSZ( HEADP(ptr) ) + GETSZ( HEADP( NEXTP(ptr))) - copySize;
			if (diffSz  < 0) {
				moreSz = MAX(-diffSz, BLKSZ);
				if (heapFix(moreSz) == NULL) { return NULL; }
				diffSz += moreSz;
			}
			removeFBLK( NEXTP(ptr) );
			INSERT( HEADP(ptr), SZALLOC(copySize + diffSz, 1) );
			INSERT( FOOTP(ptr), SZALLOC(copySize + diffSz, 1) );
			 
		} else {
			newptr = mm_malloc(copySize - DOUBSZ);
			memcpy(newptr, ptr, MIN(size, copySize));
			mm_free(ptr);
		}
		buff = GETSZ( HEADP(newptr) ) - copySize;
	}
	if (buff < 2 * 128) {
		GET( HEADP( NEXTP(newptr) ) ) |= 0x2;	// set tag
	}*/
	
	if (((void*)((char*)currP + currP->size) <= mem_heap_hi())) {
		if (!((nextP->size) & 1) && (nextP->size + PCOMP(currP) > newSz)) {
			GETSZ(currP, (currP->size + nextP->size) | 1);
			HEADP(currP, currP);
			PDELT(nextP);
			return ptr;
		}
	} else {
		mem_sbrk(newSz - (currP->size));
		GETSZ(currP, newSz);
		HEADP(currP, currP);
		return ptr;
	}
	
	void* newptr = mm_malloc(size);
	memcpy(newptr, ptr, (currP->size - SIZE_T_SIZE - ALIGN(sizeof(blkFoot))));
	mm_free(ptr); 

  return newptr;

}
