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
//Disclaimer, I got the set bit and clear and check bit from stackoverflow 
//bitmap[i] |=1<<i;		set
//bitmap[i] &= ~(1<<i);		clear
//bitmap[i] & 1<<i;		check, put in if statement

//the structs are based on professor's code for the partition in the fsLow.c
/*typedef struct VCB{
	char VCBPrefix[sizeof(H_CAPTION)+2];	//my helpppppp message
	uint64_t mnum;				//magic number maybe forgot what it was
	uint64_t signu;				//signature forgot what it was
	uint64_t nblkt;				//words for num of blocks to see in hexdump
	uint64_t nblk;				//number of blocks
	uint64_t sblkt;				//words for size of blocks to see in hexdump
	uint64_t sblk;				//size of block
	uint64_t sroott;			//words for root dir to see in hexdump
	uint64_t sroot;				//where does root dir start
	uint64_t sfst;				//words for free space to see in hexdump
	uint64_t sfs;				//where does free space start
	uint64_t sffst;				//words for free space size
	uint64_t sffs;				//size of free space map
	
}VCB_t, *VCB_p;*/

/*typedef struct bitmap{
	char * val;
}bitmap_t, *bitmap_p;*/

//based on the struct provided in the Tuesday lecture 4/13
/*typedef struct de{
	uint64_t ford;				//file (0) or directory (1)
	uint64_t size;				//size of de
	uint64_t loc;				//where is de
	uint64_t namet;				//to see name on hexdump
	char  dename [8];			//name of de
}de_t, *de_p;*/

//de_p dep = NULL;
//de_p * deaa = NULL;

de_t * dea = NULL;
VCB_p vcbp = NULL;				//initailizing this so call on this for fields
//bitmap_p bmp = NULL;
char * bitmap;					//use this to call on bitmap
//char * buffy;

//to init the VCB, the commented out part also works but I couldn't call on that in other
//functions and would get a segmentation fault when I did
//I called this function from the fsLowDemo.c right under where it calls the start partition
//I'm unsure if that is correct or if I'm supposed to call it in the start partition function

void init_VCB_blk(uint64_t  nblk, uint64_t  sblk){
printf("---------------------------inside the init_VCB_blk------------------------\n");
	vcbp = malloc(sblk);			//vcb is size of one block
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
	strcpy(vcbp -> VCBPrefix, H_CAPTION);		//all -> is setting stuff
	vcbp -> mnum = 0x004b6e6169766956;		// X -> Y = Z; is
	vcbp -> sblkt = 0x7a69736b636f6c62;		// (*X).Y = Z;
	vcbp -> sblk = sblk;				
	vcbp -> nblkt = 0x6d756e6b636f6c62;
	vcbp -> nblk = nblk/sblk;
	uint64_t written = init_free_space(vcbp->nblk,sblk);//start freespace
printf("checking where did bitmap change %d\n",bitmap[0]);
	vcbp -> sfs = 1;				//due to vcb in 0, free starts at 1
printf("what is written: %ld\n",written);
	vcbp -> sroot = written+1;			//should be where free ends +1
printf("what is sroot: %ld\n",vcbp->sroot);
	//char*rname = '.';
	//char*pname = "..";
	uint64_t der = createDir("NULL",0);		//I'm unsure about the directory
printf("checking where did bitmap change %d\n",bitmap[0]);
	//uint64_t der2 = createDir("..",1);
printf("checking where did bitmap change %d\n",bitmap[0]);
	vcbp -> sroott = 0x74726964746f6f72;
	vcbp -> sfst = 0x6361707365657266;
	vcbp -> signu = Signu;
	vcbp -> sffst = 0x657a697365657266;
	
	//int ret = allocate_free_space(2);		//please test
	//printf("what is the return of allocate_free_space %d\n",ret);
	//release_free_space(1,1);			//please test
printf("what is bitmap[0] in vcb func %d\n",bitmap[0]);
//int cou = allocate_free_space(3);			//please test
//printf("what is the return from allocate %d\n",cou);
	LBAwrite(vcbp, 1, 0);		//writing the vcb, that's the red rectangle I have been
					//screenshotting
//printf("what is in dea[0] %s, %ld\n dea[1] %s, %ld\n",dea[0]->name,dea[0]->loc, dea[1]->name,dea[1]->loc);
	free(vcbp);			//free the buffers
	free(bitmap);
	free(dea);
					//I should be freeing everything here right?
}

//init the free space

uint64_t init_free_space(){
printf("inside the init_free_space\n");
printf("what is inside vcbp:\nnumblks %ld\nsizeblk %ld\n",vcbp->nblk, vcbp->sblk);
	uint64_t bytesN = (vcbp -> nblk/8);		//bc there's 8 bits per byte
	uint64_t blksneed;
if((bytesN/vcbp->sblk)==0){
	blksneed = 1;
}else if((bytesN%vcbp->sblk)!=0){
	blksneed = (bytesN/vcbp->sblk)+1;
}
	vcbp->sffs = blksneed;
//uint64_t blksneed = ((bytesN/vcbp->sblk)==0)?1:(bytesN/vcbp-> sblk);
printf("what is blksneeded for freespace %ld\n",blksneed);
printf("b4 the calloc of free space\n");
	bitmap = (char*)calloc((blksneed*(vcbp->sblk)),sizeof(char));
	//bitmap_p bmp = malloc(blksneed*sblk);
printf("b4 the for loop of the free space is bitmap null?%s\n",bitmap);
	//bitmap[0] |= 1<<0;				//for vcb
//This was a test to see if it shows, remove later
	//for vcb block and freespace map block(s)
	for(int i = 0; i<=blksneed; i++){

		bitmap[0] |= 1<<i;
printf("How many times is this running %d\nand what is bitmap[0] %d\n",i,bitmap[0]);
		
	}
printf("wat is bitmap[0] = %c",bitmap[0]);
	
	uint64_t wrote = LBAwrite(bitmap,blksneed,1); //write the bitmap
printf("b4 the return of free space\n");
	return (wrote);				//I'm not sure what it should return
							//if it return how many blks written
							//then it's easier for root dir start
}

//find if there is continous nblksn amount of blocks free for use
int allocate_free_space(int nblksn){
printf("------------inside of the allocate free space ----------------------\n");
	int start = 0;				//track starting block index
	int lasti = 0;				//track where it left at to get continous blks
	int count = 0;				//track if the count matches blksneeded

	for(int i = 0; i<(vcbp->nblk/8); i++){	//outer loop for index of bitmap
		for(int j = 0; j < 8; j++){	//inner loop for index of bitmap[index]
			if(start == 0){
				start = (8*i)+j;
				lasti = i;
			}
			if(((bitmap[i] >> j) & 1)==1){	//check if it's been set, if so we don't want it				
printf("Did it check the first 2 bits?");				
				count = 0;
				start = 0;
			}else{			//else start tracking unset bits
				//start = (8*i)+j;
				count++;
			}
			if(count == nblksn){	//if count=blksneeded then return where that 
//setting them to 1 going in reverse.
printf("insdie the allocte free space. count == nblksn\n");
//this is what I intended the set bit to do
				int setc = 0;
				int seti =i;
				int cc = j;
				while(setc < nblksn){
printf("round %d: what is i %d: what is cc %d\n",setc,i,cc);
					bitmap[i] |= 1 << cc;
					cc--;
					setc++;
					if(cc==-1){
						cc=7;
						seti--;
					}
					
				}
				LBAwrite(bitmap,vcbp -> sffs, vcbp -> sfs);
printf("-----------------end of successsful allocate free -------------------\n");
				return start;	//address started
			}
		}
	}
printf("-----------------end of unsuccessful allocate free-----------------\n");
	return 0;				//return 0 if no space or error?
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
	uint64_t wroten = LBAwrite(bitmap,vcbp->sffs,vcbp->sfs);
	
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
	//return;???? not sure
	/*for(int i = 0; i<8; i++){
		bitmap[0] &= ~(0<<i);
	}*/
}
 
//create DE, put into DE array, LBA write, mark them out in free space 
//pretty sure i'm not doing this one right.
uint64_t createDir(char *name ,uint64_t i){
printf("-----------------inside the create directory-----------------------\n");
	int wherestart = allocate_free_space(2);
printf("after allocate\n");
	dea = malloc(2*(vcbp->sblk));
printf("after malloc of create Dir\n");
	int wherestop = (2*(vcbp->sblk))/sizeof(de_t);
printf("b4 the the . and ..\n");
char*testnull = "NULL\0";
printf("name %s | testnull %s\n",name,testnull);
	if(strcmp(name,testnull)==0){
printf("Didi i even make it into this if statment in create dir \n");
	char*child = ".";
printf("where seg fault after child?\n");
	char*parent = "..";
	//int * i = 0;
printf("where seg fault after parent?\n");
		//dea[0].name = child;
		strcpy(dea[0].dename,".");
printf("where seg fault after setting?\n");
		dea[0].loc = 0; 
		dea[0].ford = 1;
		dea[0].size = wherestop*sizeof(de_t);
		dea[0].namet = 0x74656d616e726964;
printf("Did it work?\n");
		strcpy(dea[1].dename,"..");
		//dea[1].name = parent;
		dea[1].namet = 0x74656d616e726964;
		dea[1].loc = 0; 
		dea[1].ford = 1;
		dea[1].size = wherestop*sizeof(de_t);
		strcpy(dea[2].dename,"istest");
		//dea[1].name = parent;
		dea[2].namet = 0x74656d616e726964;
		dea[2].loc = 3; 
		dea[2].ford = 1;
		dea[2].size = wherestop*sizeof(de_t);
	}else{
		//find free open 
		//init
	}
printf("I don't see the values testing fields\n");
printf("name %s | loc %ld | ford %ld |size %ld\n",dea[0].dename,dea[0].loc,dea[0].ford,dea[0].size);
printf("name %s | loc %ld | ford %ld |size %ld\n",dea[1].dename,dea[1].loc,dea[1].ford,dea[1].size);
printf("after the . and ..\n");
	LBAwrite(dea, 2, vcbp->sroot);
printf("where am I writing to? %ld\n",vcbp->sroot);
	/*for(int i = 2; i<wherestop; i++){
		deaa[i] -> name = NULL;
		deaa[i] -> loc = i; 
		deaa[i] -> ford = 1;
		deaa[i] -> size = wherestop*sizeof(de_t);
	}*/
//below was what I tried b4 the tuesday class 4/13 vid
	/*dep = malloc(sizeof(dep));		//make dep 
	dep -> name = name;			//set fields
	dep -> loc = 0x746f72646c696863;	//supposed to be i, think this was childrot temp
	dea = calloc(vcbp -> sblk,sizeof(de_p));//set directory entry array
	dea[i] = dep;				//put the directory entry into the array?
	//set_free_space(vcbp->sfs,1);
	uint64_t ind = LBAwrite(dep,1,vcbp->sroot);*/
	//free(dea);
printf("---------------end of create directory -------------------\n");
	return 0;			
}
