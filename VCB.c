/**************************************************************
* Class:  CSC-415-01 Spring 2021
* Name: Annie Liao, Vivian Kuang, Elyssa Tapawan, Joanne Wong
* Student ID: 918266744, 918403595, 918459248, 918441685
* Project: Basic File System
*
* File: VCB.c
*
* Description: This is to init the VCB and other functions
*	       to keep track of data needed in the VCB.
*
**************************************************************/

#include "VCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

de_t * dea = NULL;
VCB_p vcbp = NULL; 	//initailizing this so call on this for fields
char * bitmap;		//use this to call on bitmap

int init_VCB_blk(uint64_t  nblk, uint64_t  sblk){
	int vol_created = getval();
	
	if (vol_created == 1){ //check if volume was created
	   return 0;
	}

	//vcbp = malloc(sblk);					//vcb is size of one block
	strcpy(vcbp -> VCBPrefix, H_CAPTION);			//all -> is setting stuff
	vcbp -> mnum = 0x004b6e6169766956;			//magic number (KnaiviV) 
								// X -> Y = Z; is (*X).Y = Z;
	vcbp -> sblkt = 0x7a69736b636f6c62;			//words for size of blocks-seen in Hexdump (ziskcolb)
	vcbp -> sblk = sblk;					//size of block
	vcbp -> nblkt = 0x6d756e6b636f6c62;			//words for number of blocks-seen in Hexdump (munkcolb)
	vcbp -> nblk = nblk/sblk;
	uint64_t written = init_free_space(vcbp->nblk,sblk);	//start freespace
	vcbp -> sfs = 1;					//due to vcb in 0, free starts at 1
	vcbp -> sroot = written+1;				//should be where free ends +1

	uint64_t der = createDir("NULL", 0);
	vcbp -> sroott = 0x74726964746f6f72;			//words for root dir-seen in Hexdump (tridtoor)
	vcbp -> sfst = 0x6361707365657266;			//words for free space-seen in Hexdump (capseerf)
	vcbp -> signu = Signu;
	vcbp -> sffst = 0x657a697365657266;			//words for free space size (eziseerf)
	LBAwrite(vcbp, 1, 0);		//writing the vcb

	return 0;
}

void close_vcb(){
	free(vcbp);	//free the buffers
	free(bitmap);
	free(dea);
}

//to check if volume exist 
int getval(){
	vcbp = malloc(512);
	//see if the volume already exit, if so, set the structs to it
	uint64_t LBA_read = LBAread(vcbp,1,0);

	//not our volume
	if (vcbp->signu == 0){
		return 0;
	}

	//check if it's our volume
	if (vcbp->signu==Signu){
		//read and set structs
		bitmap = (char*)calloc(((vcbp->sffs)*(vcbp->sblk)),sizeof(char));
		LBAread(bitmap,vcbp->sffs,vcbp->sfs);
		dea = malloc(10*(vcbp->sblk));
		LBAread(dea,vcbp->srootbs,vcbp->sroot);
		return 1;
	}

	return 0;
}

//init free space
uint64_t init_free_space(){
	uint64_t bytesN = (vcbp -> nblk/8);	//there's 8 bits per byte
	uint64_t blksneed;

	//check how much blocks needed for bitmap
	if ((bytesN/vcbp->sblk)==0){
		blksneed = 1;
	} else if ((bytesN%vcbp->sblk)!=0){
		blksneed = (bytesN/vcbp->sblk)+1;
	}

	vcbp->sffs = blksneed;
	bitmap = (char*)calloc((blksneed*(vcbp->sblk)),sizeof(char));

	//set to zero
	for(int j = 0; j< (bytesN); j++){
		for(int k = 0; k < 8; k++){
			bitmap[j] |= 0 << k;
		}
	}

	//set the bitmap in the bitmap including vcb
	for(int i = 0; i<=blksneed; i++){
		bitmap[0] |= 1<<i;
	}

	uint64_t LBA_write = LBAwrite(bitmap,blksneed,1); //write the bitmap

	return (LBA_write);			
}

//find if there is continous nblksn amount of blocks free for use
int allocate_free_space(int nblksn){
	int start = -1;				//track starting block index
	//int lasti = 0;			//track where it left at to get continous blks
	int count = 0;				//track if the count matches blksneeded

	for(int i = 0; i < vcbp->nblk/8; i++){			//go through bitmap array
		for(int j = 0; j < 8; j++){			//go through bits of the char in bitmap
			if((bitmap[i] & (1 << j)) == 0){	//check for unset bit
				if(start == -1){		//check if start is set
					start = (i * 8) + j;    //set start block number
				}

				bitmap[i] |= 1 << j;		//set the bit
				count ++;			//update count
			//lasti = (i*8)+j;
				if(count == nblksn){		//if enough continous block return start and write the changes
					LBAwrite(bitmap,vcbp -> sffs, vcbp -> sfs);
					return start;
				}
			} else {				//if here, bit was set and there's a break in the continuity
				if(start > 0){
					start = -1;		//start back at -1
					count = 0;		//undo count
				}
			}
		}
	}

	return start;
}

//unset the bits starting at LBA til count
void release_free_space(int LBA, int count){
	int index = (LBA / 8);			//each index of bitmap holds 8 so LBA/8 gives idx
	int innerindex = (LBA % 8);		//need the innerindex of the bitmap[idx]
	int track = 0;				//to keep track of how much cleared

	while (track < count){ 
		bitmap[index] &= ~(1<<innerindex);	//clear bit at inneridx of the bitmap[idx]
		track++;				//update track
		innerindex++;			//update innerindex
	
		if(innerindex == 8){		//check if it's 8 bc then we reach end of inneridx
			innerindex = 0;		//set to 0 to start new inneridx
			index++;		//update idx to match the new inneridx
		}
	}

	uint64_t LBA_write1 = LBAwrite(bitmap,vcbp->sffs,vcbp->sfs); //write changes to bitmap
}

uint64_t makede(char*fname, uint64_t index,uint64_t sz,uint64_t fod, char *currentDir){

	for (int i = 0; i<vcbp->sroots; i++){
        	if (dea[i].size == -1){			//find free spot in de array
			strcpy(dea[i].dename,fname);
			dea[i].namet = 0x74656d616e726964;	//name on hex dump (temanrid)
			dea[i].loc = index; 			//location of directory entry
			dea[i].ford = fod;			//file (0) or directory (1)
			dea[i].size = sz;			//size of directory entry
			strcpy(dea[i].currentDir, currentDir);//copy name of dir
 			LBAwrite(dea, 10, vcbp->sroot);	//write changes
			return i;
        	}
    	}
    return 0;
}

//create DE, put into DE array, LBA write, mark them out in free space 
uint64_t createDir(char *name ,uint64_t i){
	//get blocks for de array
	uint64_t where_start = allocate_free_space(10);
	dea = malloc(10*(vcbp->sblk));				//10 blocks for the de array
	vcbp->srootbs = 10;					//set de array size in vcb
	uint64_t where_stop = (10*(vcbp->sblk))/sizeof(de_t);	//how many de can fit in de array
	vcbp->sroots = where_stop;				//set how many fit
	char*test_null = "NULL\0";				//check null
	
	if (strcmp(name,test_null)==0){				//check if it's for init
		for (int g = 0; g<where_stop; g++){		//init size of de to -1
			dea[g].size = -1;
		}
	    //setting of the child and parent of root
	    strcpy(dea[0].dename,".");
	    dea[0].loc = 0; 
	    dea[0].ford = 1;
	    dea[0].size = 0;
	    dea[0].namet = 0x74656d616e726964;	//temanrid

	    strcpy(dea[1].dename,"..");
	    dea[1].namet = 0x74656d616e726964;	//temanrid
	    dea[1].loc = 0; 
	    dea[1].ford = 1;
	    dea[1].size = 0;

	}
	//have to write those changes
	LBAwrite(dea, 10, vcbp->sroot);
	return where_start;
}
