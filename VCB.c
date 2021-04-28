/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: VCB.c
*
* Description: this is to init the vcb and other functions
*		to keep track of data needed in the VCB.
*Disclaimer, I got the set bit and clear and check bit 
*from stackoverflow, I'm sorry, my memory of CSC 256 is 
*a bit fuzzy
*bitmap[i] |=1<<i;		set
*bitmap[i] &= ~(1<<i);		clear
*bitmap[i] & 1<<i;		check, put in if statement
*
*I knew how to make a struct but the structs and how I init 
*them are based on professor's code for the partition in 
*the fsLow.c, I'm sorry
**************************************************************/
//#include <stdint.h>
#include "VCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include "fsLow.h"
#include <sys/types.h>
#include <unistd.h>


de_t * dea = NULL;
VCB_p vcbp = NULL;				//initailizing this so call on this for fields
char * bitmap;					//use this to call on bitmap


//to init the VCB, the commented out part also works but I couldn't call on that in other
//functions and would get a segmentation fault when I did
//I called this function from the fsLowDemo.c right under where it calls the start partition
//I'm unsure if that is correct or if I'm supposed to call it in the start partition function

int init_VCB_blk(uint64_t  nblk, uint64_t  sblk){
printf("---------------------------inside the init_VCB_blk------------------------\n");
	int nd = getval();
	if(nd == 1){printf("end early already init-ed\n");return 0;}
	//vcbp = malloc(sblk);			//vcb is size of one block
	
	
	strcpy(vcbp -> VCBPrefix, H_CAPTION);		//all -> is setting stuff
	vcbp -> mnum = 0x004b6e6169766956;		// X -> Y = Z; is
	vcbp -> sblkt = 0x7a69736b636f6c62;		// (*X).Y = Z;
	vcbp -> sblk = sblk;				
	vcbp -> nblkt = 0x6d756e6b636f6c62;
	vcbp -> nblk = nblk/sblk;
	uint64_t written = init_free_space(vcbp->nblk,sblk);//start freespace
//printf("checking where did bitmap change %d\n",bitmap[0]);
	vcbp -> sfs = 1;				//due to vcb in 0, free starts at 1
//printf("what is written: %ld\n",written);
	vcbp -> sroot = written+1;			//should be where free ends +1

	uint64_t der = createDir("NULL",0);		//I'm unsure about the directory

	vcbp -> sroott = 0x74726964746f6f72;
	vcbp -> sfst = 0x6361707365657266;
	vcbp -> signu = Signu;
	vcbp -> sffst = 0x657a697365657266;
	
printf("writing at vcbp\n");
	LBAwrite(vcbp, 1, 0);		//writing the vcb, that's the red rectangle I have been
					//screenshotting
//printf("what is in dea[0] %s, %ld\n dea[1] %s, %ld\n",dea[0]->name,dea[0]->loc, dea[1]->name,dea[1]->loc);
	//free(vcbp);			//free the buffers
	//free(bitmap);
	//free(dea);
printf("what is bitmap in initvcb %s\n",bitmap);
	printf("endd late. was not init-ed\n");
	return 0;
					//I should be freeing everything here right?
}
void close_vcb(){
	free(vcbp);			//free the buffers
	free(bitmap);
	free(dea);
}
//init the free space
int getval(){
	
printf("insie the getval\n");
	//int pls = getdval();
//printf("out of teh getdval in the getval\n");
	vcbp = malloc(512);
//printf("after the malloc b4 read\n");
	uint64_t fr = LBAread(vcbp,1,0);
	if(vcbp->signu == 0){
		//free(vcbp);
		return 0;
	}
	if(vcbp->signu==Signu){
		//vcbp = malloc(buff->sblk);
		//vcbp = buff;

		bitmap = (char*)calloc(((vcbp->sffs)*(vcbp->sblk)),sizeof(char));
		//char * getb = malloc((vcbp->sblk)*(vcbp->sffs));
		//char*getb = (char*)calloc((blksneed*(vcbp->sblk)),sizeof(char));
		LBAread(bitmap,vcbp->sffs,vcbp->sfs);
		//bitmap = malloc((vcbp->sblk)*(vcbp->sffs));
		//bitmap = getb;
		//memcpy(bitmap,getb,((vcbp->sblk)*(vcbp->sffs)));

		//de_t * getd = malloc((vcbp->sblk) * (vcbp->srootbs));
		dea = malloc(2*(vcbp->sblk));
		LBAread(dea,vcbp->srootbs,vcbp->sroot);
		//dea = malloc((vcbp->sblk) * (vcbp->srootbs));
		//memcpy(dea,getd,(vcbp->sblk) * (vcbp->srootbs));
		
		
		printf("@@@@@@@@@@@@@@@@@@@inside getval what is sroots %ld\n",vcbp->sblk);
		printf("what is the lba i of dea %ld\n",vcbp->sroot);
		printf("what is the sfs %ld\n",vcbp->sfs);
		printf("what is size of a dea %ld\n",dea[4].size);
		printf("what is bitmap in the getval %s\n",bitmap);	
		
		return 1;
	}
	
	

	//printf("what is buff %s\n",buff);
	return 0;
}
uint64_t init_free_space(){
printf("inside the init_free_space\n");
//printf("what is inside vcbp:\nnumblks %ld\nsizeblk %ld\n",vcbp->nblk, vcbp->sblk);
	uint64_t bytesN = (vcbp -> nblk/8);		//bc there's 8 bits per byte
	uint64_t blksneed;
	if((bytesN/vcbp->sblk)==0){
		blksneed = 1;
	}else if((bytesN%vcbp->sblk)!=0){
		blksneed = (bytesN/vcbp->sblk)+1;
	}
	vcbp->sffs = blksneed;

	bitmap = (char*)calloc((blksneed*(vcbp->sblk)),sizeof(char));

	for(int j = 0; j< (bytesN); j++){
		for(int k = 0; k < 8; k++){
			bitmap[j] |= 0 << k;
		}
	}

	for(int i = 0; i<=blksneed+1; i++){

		bitmap[0] |= 1<<i;
		
	}
printf("writing at bitmap\n");
	uint64_t wrote = LBAwrite(bitmap,blksneed,1); //write the bitmap

	return (wrote);				//I'm not sure what it should return
							//if it return how many blks written
							//then it's easier for root dir start
}

//find if there is continous nblksn amount of blocks free for use
int allocate_free_space(int nblksn){
printf("------------inside of the allocate free space ----------------------\n");
printf("%s\n",bitmap);
	int start = 0;				//track starting block index
	int lasti = 0;				//track where it left at to get continous blks
	int count = 0;				//track if the count matches blksneeded
	for(int v = 0; v < (vcbp->nblk); v++){
printf("howmmany times do the for loop run.? %d\n",v);
		int widx = log(bitmap[v])/log(2);

		start = widx;
		if(widx < 7){
			while(nblksn > count){
printf("what is v %d\n what is widx %d\n",v,widx);
				bitmap[v] |= 1 << widx;
				count++;
				widx++;
				if(widx == 7){v++; widx = 0;}
			}
		}
		if(nblksn == count){break;}

		
	}
printf("what is sfs %ld and sffs %ld\n",vcbp->sfs, vcbp->sffs);
if(start > 3){printf("what id dea size %ld\n",dea[4].size);}
		LBAwrite(bitmap,vcbp -> sffs, vcbp -> sfs);
printf("what is sfs %ld and sffs %ld\n",vcbp->sfs, vcbp->sffs);
if(start > 3){printf("what id dea size %ld\n",dea[4].size);}
printf("########### what is the start given %d\n",start);
		return start;
}

//Dunno if we are actually going to this one
//set the bits starting at LBA til count, not sure about this one
//seems like it could be done in allocate but then we should actually check if the block
//has been written to. if this, then either we gotta store more values or put more paramters
void set_free_space(int LBA, int count){
	int idx = (LBA/8);			//each index of bitmap holds 8 so LBA/8 gives idx
	int inneridx = (LBA%8)-1;			//need the innerindex of the bitmap[idx]
	int trk = 0;				//to keep track of how much cleared
	while(trk < count){
		 
		bitmap[idx] |= (1<<inneridx);	//clear bit at inneridx of the bitmap[idx]
		trk++;				//update trk
		inneridx++;			//update inneridx
		if(inneridx == 8){		//check if it's 8 bc then we reach end of inneridx
			inneridx = 0;		//set to 0 to start new inneridx
			idx++;			//update idx to match the new inneridx
		}
	}
	//uint64_t wroten = LBAwrite(bitmap,vcbp->sffs,vcbp->sfs);
	
}

//unset the bits starting at LBA til count
void release_free_space(int LBA, int count){
printf("-------------start of release free space ----------------\n");
	int idx = (LBA/8);			//each index of bitmap holds 8 so LBA/8 gives idx
	int inneridx = (LBA%8)-1;			//need the innerindex of the bitmap[idx]
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
	uint64_t wroten = LBAwrite(bitmap,vcbp->sffs,vcbp->sfs);
printf("----------end of release free space-----------------\n");
}

uint64_t makede(char*fname, uint64_t idx,uint64_t sz){
printf("what is the sroots %ld\n",vcbp->sroots);
	for(int i = 0; i<vcbp->sroots; i++){
printf("if it ain't 0 then wat is it? %ld\n",dea[i].size);
		if(dea[i].size == 0){
printf("trying to set a dirctory here n makede\n");
			strcpy(dea[i].dename,fname);
			dea[i].namet = 0x74656d616e726964;
			dea[i].loc = idx; 
			dea[i].ford = 1;
			dea[i].size = sz;
			LBAwrite(dea, 2, vcbp->sroot);
			return i;
		}
	}
	return 0;
}

//create DE, put into DE array, LBA write, mark them out in free space 
//pretty sure i'm not doing this one right.
uint64_t createDir(char *name ,uint64_t i){
printf("-----------------inside the create directory-----------------------\n");
	uint64_t wherestart = allocate_free_space(3);
printf("+/+/+/+/+/+/what is where start %ld \nafter allocate\n",wherestart);
	dea = malloc(2*(vcbp->sblk));
	vcbp->srootbs = 2;
	uint64_t wherestop = (2*(vcbp->sblk))/sizeof(de_t);
	vcbp->sroots = wherestop;

	char*testnull = "NULL\0";
	
	if(strcmp(name,testnull)==0){
		for(int g = 0; g<wherestop; g++){
			dea[g].size = 0;
		}

		strcpy(dea[0].dename,".");
		dea[0].loc = 0; 
		dea[0].ford = 1;
		dea[0].size = 2;
		dea[0].namet = 0x74656d616e726964;

		strcpy(dea[1].dename,"..");
		dea[1].namet = 0x74656d616e726964;
		dea[1].loc = 0; 
		dea[1].ford = 1;
		dea[1].size = 2;
//start of test
		strcpy(dea[2].dename,"istest");
		dea[2].namet = 0x74656d616e726964;
		dea[2].loc = 4; 
		dea[2].ford = 0;
		dea[2].size = 1;
		char*ttbb = malloc(512);
		strcpy(ttbb,tblk);
		uint64_t reh = LBAwrite(ttbb,1,4);
		if(reh == 0){printf("-------------write didn't work-----------------\n");}
		free(ttbb);
//end of test
	}
	LBAwrite(dea, 2, vcbp->sroot);
printf("---------------end of create directory -------------------\n");
	return wherestart;			
}
