/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: VCB.c
*
* Description: This is a demo to show how to use the fsLow
* 	routines.
*
**************************************************************/
//#include <stdint.h>
#include "VCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "fsLow.h"
#include <sys/types.h>
typedef struct VCB{
	char VCBPrefix[sizeof(H_CAPTION)+2];
	uint64_t mnum;
	uint64_t signu;
	uint64_t nblkt;
	uint64_t nblk;
	uint64_t sblkt;
	uint64_t sblk;
	uint64_t sroott;
	uint64_t sroot;
	uint64_t sfst;
	uint64_t sfs;
	
}VCB_t, *VCB_p;

/*typedef struct bitmap{
	char * val;
}bitmap_t, *bitmap_p;*/

typedef struct de{
	
	uint64_t loc;
	char  *name;
}de_t, *de_p;

de_p dep = NULL;
de_p * dea = NULL;
VCB_p vcbp = NULL;
//bitmap_p bmp = NULL;
char * bitmap;
//char * buffy;
void init_VCB_blk(uint64_t  nblk, uint64_t  sblk){
printf("inside the init_VCB_blk\n");
	vcbp = malloc(sblk);
	//VCB_p bufv = malloc(sblk);
	/*strcpy(bufv -> VCBPrefix, H_CAPTION);
	bufv -> mnum = 0x004b6e6169766956;
	bufv -> sblkt = 0x7a69736b636f6c62;
	bufv -> sblk = sblk;
	bufv -> nblkt = 0x6d756e6b636f6c62;
	bufv -> nblk = nblk/sblk;
	uint64_t written = init_free_space(bufv->nblk,sblk);
	bufv -> sfs = 1;
	
	bufv -> sroot = written+1;
	uint64_t der = createDir(".",0,bufv->sroot);
	uint64_t der2 = createDir("..",1,bufv->sroot);
	bufv -> sroott = 0x74726964746f6f72;
	bufv -> sfst = 0x6361707365657266;
	bufv -> signu = Signu;
	LBAwrite(bufv, 1, 0);
	free(bufv);*/
	strcpy(vcbp -> VCBPrefix, H_CAPTION);
	vcbp -> mnum = 0x004b6e6169766956;
	vcbp -> sblkt = 0x7a69736b636f6c62;
	vcbp -> sblk = sblk;
	vcbp -> nblkt = 0x6d756e6b636f6c62;
	vcbp -> nblk = nblk/sblk;
	uint64_t written = init_free_space(vcbp->nblk,sblk);
	vcbp -> sfs = 1;
printf("what is written: %ld\n",written);
	vcbp -> sroot = written+1;
printf("what is sroot: %ld\n",vcbp->sroot);
	//char*rname = '.';
	//char*pname = "..";
	uint64_t der = createDir(".",0);
	uint64_t der2 = createDir("..",1);
	vcbp -> sroott = 0x74726964746f6f72;
	vcbp -> sfst = 0x6361707365657266;
	vcbp -> signu = Signu;
	LBAwrite(vcbp, 1, 0);

	free(vcbp);
}


uint64_t init_free_space(){
//printf("inside the init_free_space\n");
	printf("what is inside vcbp:\nnumblks %ld\nsizeblk %ld\n",vcbp->nblk, vcbp->sblk);
	uint64_t bytesN = (vcbp -> nblk/8);
	uint64_t blksneed = 1;
//printf("b4 the calloc of free space\n");
	bitmap = (char*)calloc((bytesN),sizeof(char));
	//bitmap_p bmp = malloc(blksneed*sblk);
//printf("b4 the for loop of the free space is bitmap null?%s\n",bitmap);
	bitmap[0] = 48;
printf("wat is bitmap[0] = %c",bitmap[0]);
	//bitmap[1/8] |=1 << (1 & 7);
	uint64_t wrote = LBAwrite(bitmap,blksneed,1);
printf("b4 the return of free space\n");
	return (wrote+1);
}

int allocate_free_space(int nblksn){
	int start = 0;				//track starting block index
	int lasti = 0;				//track where it left at to get continous blks
	int count = 0;				//track if the count matches blksneeded

	for(int i = 0; i<(vcbp->nblk/8); i++){	//outer loop for index of bitmap
		for(int j = 0; j < 8; j++){	//inner loop for index of bitmap[index]
			if(bitmap[i] & 1<<j){	//check if it's been set, if so we don't want it
				count = 0;
			}else{			//else start tracking unset bits
				start = (8*i)+j+1;
				count++;
			}
			if(count == nblksn){	//if count=blksneeded then return where that 
				return start;	//address started
			}
		}
	}
	return 0;				//return 0 if no space or error?
}

void release_free_space(int LBA, int count){
	int idx = (LBA/8);			//each index of bitmap holds 8 so LBA/8 gives idx
	int inneridx = (LBA%8);			//need the innerindex of the bitmap[idx]
	int trk = 0;				//to keep track of how much cleared
	while(trk < count){
		bitmap[idx] &= ~(1<<inneridx);	//clear bit at inneridx of the bitmap[idx]
		trk++;				//update trk
		inneridx++;			//update inneridx
		if(inneridx == 8){		//check if it's 8 bc then we reach end of inneridx
			inneridx = 0;		//set to 0 to start new inneridx
			idx++;			//update idx to match the new inneridx
		}
	}
	//return;????
}

void allocate_dir(){

}

//create DE, put into DE array, LBA write, mark them out in free space
uint64_t createDir(char *name ,uint64_t i){
	dep = malloc(sizeof(dep));
	dep -> name = name;
	dep -> loc = 0x726964746f6f72;
	dea = calloc(vcbp -> sblk,sizeof(de_p));
	dea[i] = dep;


	uint64_t ind = LBAwrite(dea,1,vcbp->sroot);
	return ind;
	
}
