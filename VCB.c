/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: VCB.c
*
* Description: this is to init the vcb and other functions
*	       to keep track of data needed in the VCB.
*
* Disclaimer: the set bit and clear and check bit are from 
* 	      from stackoverflow 
*
* bitmap[i] |=1<<i;		set
* bitmap[i] &= ~(1<<i);		clear
* bitmap[i] & 1<<i;		check, put in if statement
*
* How structs were made and they were inited were based off
* the professor's code for the partition in fsLow.c 
*
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
VCB_p vcbp = NULL; 	//initailizing this so call on this for fields
char * bitmap;		//use this to call on bitmap


//to init the VCB, the commented out part also works but I couldn't call on that in other
//functions and would get a segmentation fault when I did
//I called this function from the fsLowDemo.c right under where it calls the start partition
//I'm unsure if that is correct or if I'm supposed to call it in the start partition function

int init_VCB_blk(uint64_t  nblk, uint64_t  sblk){
	printf("---------------------------inside the init_VCB_blk------------------------\n");
	int vol_created = getval();
	
	if (vol_created == 1){ //check if volume was created
	   printf("End early, already init-ed\n");
	   return 0;
	}
	//vcbp = malloc(sblk);			//vcb is size of one block

	strcpy(vcbp -> VCBPrefix, H_CAPTION);	//all -> is setting stuff
	vcbp -> mnum = 0x004b6e6169766956;	//magic number (KnaiviV) 
						// X -> Y = Z; is (*X).Y = Z;
	vcbp -> sblkt = 0x7a69736b636f6c62;	//words for size of blocks-seen in Hexdump (ziskcolb)
	vcbp -> sblk = sblk;			//size of block
	vcbp -> nblkt = 0x6d756e6b636f6c62;	//words for number of blocks-seen in Hexdump (munkcolb)
	vcbp -> nblk = nblk/sblk;
	uint64_t written = init_free_space(vcbp->nblk,sblk);	//start freespace

	//printf("checking where did bitmap change %d\n",bitmap[0]);
	vcbp -> sfs = 1;				//due to vcb in 0, free starts at 1
	//printf("what is written: %ld\n",written);
	vcbp -> sroot = written+1;			//should be where free ends +1

	uint64_t der = createDir("NULL",0);		//I'm unsure about the directory

	vcbp -> sroott = 0x74726964746f6f72;	//words for root dir-seen in Hexdump (tridtoor)
	vcbp -> sfst = 0x6361707365657266;	//words for free space-seen in Hexdump (capseerf)
	vcbp -> signu = Signu;
	vcbp -> sffst = 0x657a697365657266;	//words for free space size eziseerf
	
	printf("Writing at vcbp\n");
	LBAwrite(vcbp, 1, 0);		//writing the vcb, that's the red rectangle I have been screenshotting
	//printf("what is in dea[0] %s, %ld\n dea[1] %s, %ld\n",dea[0]->name,dea[0]->loc, dea[1]->name,dea[1]->loc);
	//free(vcbp);			//free the buffers
	//free(bitmap);
	//free(dea);

	printf("Bitmap in initvcb: %s\n",bitmap);
	printf("End late, was not init-ed\n");
	return 0;
					//I should be freeing everything here right?
	}

void close_vcb(){
	free(vcbp);	//free the buffers
	free(bitmap);
	free(dea);
}
//init the free space

int getval(){
	printf("-----------------------------inside the getval()--------------------------\n");
	vcbp = malloc(512);

	uint64_t LBA_read = LBAread(vcbp,1,0);

	if (vcbp->signu == 0){
	   //free(vcbp);
	   return 0;
	}

	if (vcbp->signu==Signu){

           bitmap = (char*)calloc(((vcbp->sffs)*(vcbp->sblk)),sizeof(char));

	   LBAread(bitmap,vcbp->sffs,vcbp->sfs);

	   dea = malloc(10*(vcbp->sblk));

	   LBAread(dea,vcbp->srootbs,vcbp->sroot);


       	//printfs for testing
	/*printf("-----------------------------inside getval()---------------------------\n");
	printf("sblk: %ld\n",vcbp->sblk);
	printf("Lba i of dea: %ld\n",vcbp->sroot);
	printf("Sfs: %ld\n",vcbp->sfs);
	printf("Size of a dea: %ld\n",dea[4].size);
	printf("Bitmap in the getval: %s\n",bitmap);	*/
		
	return 1;
	}
	
	//printf("what is buff %s\n",buff);
	return 0;
}

uint64_t init_free_space(){
	printf("----------------------------inside the init_free_space-----------------------\n");
	//printf("what is inside vcbp:\nnumblks %ld\nsizeblk %ld\n",vcbp->nblk, vcbp->sblk);
	uint64_t bytesN = (vcbp -> nblk/8);	//there's 8 bits per byte
	uint64_t blksneed;
	
	if ((bytesN/vcbp->sblk)==0) {
	   blksneed = 1;
	} else if ((bytesN%vcbp->sblk)!=0){
	   blksneed = (bytesN/vcbp->sblk)+1;
	}

	vcbp->sffs = blksneed;
	bitmap = (char*)calloc((blksneed*(vcbp->sblk)),sizeof(char));

	for(int j = 0; j< (bytesN); j++){
		for(int k = 0; k < 8; k++){
		   bitmap[j] |= 0 << k;
		}
	}

	for(int i = 0; i<=blksneed; i++){
//printf("/-/-/-/-/-/-/-/-/-/-/ init free what is i %d\n",i);
	   bitmap[0] |= 1<<i;
	}

	printf("writing at bitmap\n");
	uint64_t LBA_write = LBAwrite(bitmap,blksneed,1); //write the bitmap

	return (LBA_write);	//I'm not sure what it should return
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
	
	for(int i = 0; i < vcbp->nblk/8; i++){
			//printf("what is bitmap at i: %d, bitmap: %d\n",i,bitmap[i]);
			int widx = (log(bitmap[i])/log(2))+1;
			
			//printf("where is it free? what is widx %d\n",widx);
			int count = 0;
			if((widx < -1) && !(bitmap[i] & (1 << 7)) != 0){widx = 0;}
			if((widx < -1) && (bitmap[i] & (1 << 7)) != 0){
//printf("b4 continue\n");
			continue;}
			if(widx < 7){
				start = (i*8)+widx;
				lasti = widx;
				while(count < nblksn){
					if((bitmap[i] & (1 << widx)) == 0){
						if(lasti == widx){
//printf("in the if what is i %d, widx %d\n",i,widx);
							bitmap[i] |= 1 << widx;
							count++;
							widx++;
							lasti = widx;
						}else{
							start = (i*8)+widx;
							lasti=widx;
							count = 0;
						}
					}
					if(widx == 8){i++; widx = 0;lasti=widx;}
				}
				if(count == nblksn){printf("what is i: %d ? bit: %d\n",i,bitmap[i]);
printf("what is actual count %d\n",((i*8)+widx));
 							break;}
			}
		}
	/*for (int v = 0; v < (vcbp->nblk); v++){
	    printf("Number of time for loop runs: %d\n",v);
	    int widx = log(bitmap[v])/log(2);
	    start = widx;	
		if (widx < 7){
			while (nblksn > count){
		              printf("V: %d\nWidx: %d\n",v,widx);
		              bitmap[v] |= 1 << widx;
		              count++;
			      widx++;
				if (widx == 7){
				   v++; 
				   widx = 0;
				}
		   	}
		}
		//DO WE NEED LASTi? I WASN'T SURE LOL???
		lasti = v;
		printf("lasti: %d\n", lasti);
		if (nblksn == count){
		   break;
		}
	}*/

	printf("Sfs: %ld and sffs: %ld\n",vcbp->sfs, vcbp->sffs);
	//if(start > 3){printf("what id dea size %ld\n",dea[4].size);}
	LBAwrite(bitmap,vcbp -> sffs, vcbp -> sfs);
	printf("Sfs: %ld and sffs: %ld\n",vcbp->sfs, vcbp->sffs);
	//if(start > 3){printf("what id dea size %ld\n",dea[4].size);}
	printf("Start given: %d\n",start);
	return start;
}

//Dunno if we are actually going to this one
//set the bits starting at LBA til count, not sure about this one
//seems like it could be done in allocate but then we should actually check if the block
//has been written to. if this, then either we gotta store more values or put more paramters
void set_free_space(int LBA, int count){
	int index = (LBA / 8);			//each index of bitmap holds 8 so LBA/8 gives idx
	int innerindex = (LBA % 8)-1;		//need the innerindex of the bitmap[idx]
	int track = 0;				//to keep track of how much cleared

	while (track < count){ 
	      	bitmap[index] |= (1<<innerindex);//clear bit at inneridx of the bitmap[idx]
		track++;			//update track
		innerindex++;			//update innerindex

		if (innerindex == 8){		//check if it's 8 bc then we reach end of innerindex
			innerindex = 0;		//set to 0 to start new innerindex
			index++;		//update idx to match the new innerindex
		}
	}
	//uint64_t wroten = LBAwrite(bitmap,vcbp->sffs,vcbp->sfs);
}

//unset the bits starting at LBA til count
void release_free_space(int LBA, int count){
	printf("-------------start of release free space ----------------\n");
	int index = (LBA / 8);			//each index of bitmap holds 8 so LBA/8 gives idx
	int innerindex = (LBA % 8)-1;		//need the innerindex of the bitmap[idx]
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
	uint64_t LBA_write1 = LBAwrite(bitmap,vcbp->sffs,vcbp->sfs);
	printf("--------------end of release free space-----------------\n");
}

uint64_t makede(char*fname, uint64_t index,uint64_t sz,uint64_t fod, char *currentDir){
	printf("Sroots: %ld\n",vcbp->sroots);

	for (int i = 0; i<vcbp->sroots; i++){
	    //printf("if its not 0 then what is it? %ld\n",dea[i].size);
        	if (dea[i].size == 0){
		   //printf("trying to set a dirctory here n makede\n");
          	   strcpy(dea[i].dename,fname);
           	   dea[i].namet = 0x74656d616e726964;	//name on hex dump (temanrid)
                   dea[i].loc = index; 			//location of directory entry
                   dea[i].ford = fod;			//file (0) or directory (1)
                   dea[i].size = sz;			//size of directory entry
                   strcpy(dea[i].currentDir, currentDir);
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
	
	uint64_t where_start = allocate_free_space(10);
	printf("------------------where is start  %ld \nafter allocate-------------\n",where_start);
	
	dea = malloc(10*(vcbp->sblk));
	vcbp->srootbs = 10;
	uint64_t where_stop = (10*(vcbp->sblk))/sizeof(de_t);
	vcbp->sroots = where_stop;
	char*test_null = "NULL\0";
	
	if (strcmp(name,test_null)==0){
		for (int g = 0; g<where_stop; g++){
		    dea[g].size = 0;
		}

	    strcpy(dea[0].dename,".");
	    dea[0].loc = 0; 
	    dea[0].ford = 1;
	    dea[0].size = 2;
	    dea[0].namet = 0x74656d616e726964;	//temanrid

	    strcpy(dea[1].dename,"..");
	    dea[1].namet = 0x74656d616e726964;	//temanrid
	    dea[1].loc = 0; 
	    dea[1].ford = 1;
	    dea[1].size = 2;

//start of test
	//     strcpy(dea[2].dename,"istest");
	//     dea[2].namet = 0x74656d616e726964;
	//     dea[2].loc = 4; 
	//     dea[2].ford = 0;
	//     dea[2].size = 1;
	//     char*bytes = malloc(512);
   	//     strcpy(bytes,tblk);
	//     uint64_t LBA_write2  = LBAwrite(bytes,1,4);
		
	// if (LBA_write2 == 0){
	//    printf("---------------------------write didn't work------------------------\n");}
	//    free(bytes);
//end of test
	}

	LBAwrite(dea, 10, vcbp->sroot);
	printf("---------------------------end of directory------------------------\n");
	return where_start;
}
