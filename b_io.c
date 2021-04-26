#include "b_io.h"
#include <stdio.h>
#include "VCB.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mfs.h"
#define maxf 20
typedef struct finfo{
	int bc;			//blockcount of file use for r/w
	int fidx;		//file index use for r/w
	char * fbuffer;		//file buffer use for r/w
	int bufferdata;
	int per;		//file permission
	int startf;		//what is the start/lba index
	int flen;		//blk len of file from that lba index
	int didx;		//index in the de array
}finfo;
int party = 0;
//finfo_p fiop = NULL;
//fdDir *fdDiri = NULL;
finfo farr [maxf];
int started = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
VCB_p vcbp;
void b_init (){
	//init fcbArray to all free
	for (int i = 0; i < maxf; i++){
		farr[i].didx = -1; //indicates a free fcbArray
	}
		
	started = 1;
}

int b_getfarr (){
	for (int i = 0; i < maxf; i++){
		if (farr[i].didx == -1){
			farr[i].didx = -2; // used but not assigned
			return i;		//Not thread safe
		}
	}
	return (-1);  //all in use
}


int b_open (char * filename, int flags){
printf("------------------inside the b_open in b_io.c-----------------\n");
if(party == 1){printf("party is 1. what is sroots %ld\n",vcbp->sroots);getval();}
//need to check if the array is init
if(started == 0){b_init();}
printf("after b_init in b-open\n");
//need to get space
int idx = b_getfarr();
if(idx == -1){printf("no more space\n"); return -1;}
printf("after the b_getfarr in b-open\n");
//need to make sure Partition is started
	char * sname = "testone";
	uint64_t vsize = 500000;
	uint64_t bsize = 512;
	
	
	if(party == 0 ){int istart = startPartitionSystem(sname, &vsize, &bsize);
if(istart != 0){printf("unable to open\n"); return -1;}
		init_VCB_blk(vsize,bsize);
		party = 1;
	}else{
printf("what is sroots %ld\n",vcbp->sroots);}
printf("after the start partition system and init_VCB_blk in b-open\n");
printf("why does it stop here?\n");
printf("what i sthe sroots %ld\n",vcbp->sroots);
//need to find filename in directory array
	for(int i = 0; i < 25; i++){
		if(strcmp(dea[i].dename, filename) == 0){
			farr[idx].didx = i;
			break;
		}
		if((i+1)  == vcbp->sroots){
			farr[idx].startf = allocate_free_space(1);
			farr[idx].didx = makede(filename,farr[idx].startf,1);
		}
	}
printf("after the for loop to find matching filename in b-open\n");
//need to initalize values from struct
	farr[idx].flen = dea[farr[idx].didx].size;
	farr[idx].startf = dea[farr[idx].didx].loc;
	farr[idx].bc = 0;
	farr[idx].fidx = 0;
	farr[idx].fbuffer = malloc(vcbp->sblk);
	farr[idx].bufferdata = 0;
printf("after the init values of struct in b-open\n");
//need to determine permissions
	if(flags = 0){farr[idx].per = 1;}
	if(flags > 0){farr[idx].per = 2;}
printf("after the setting per in b-open\n");
//need to return farr index
//int retv = allocate_free_space(3);
//printf("what is allocate %d\n",retv);

//need to make one if none

	return idx;
}

int b_read (int fd, char * buffer, int count){
printf("-----------inside the b_read in b_io.c------------\n");
printf("what' swrong with blksize %ld\n",vcbp->sblk);
	if(farr[fd].per == -1){printf("ERROR: file was closed. file not opened\n"); return 0;}
	//keep track of how much data is copied to the buffer
		int dataCopied = 0;

		//loop makes sure that enough chars are being copied into the buffer, it will 			
		//stop once the requested amount is filled or EOF
		while(dataCopied != count){

			//checking the amount in the file buffer, will execute a read to fill 	
			//file buffer if it's 0
			if(farr[fd].fidx == 0){
//printf("what is sblk %ld\n",vcbp->sblk);
				uint64_t savesblk = vcbp->sblk;
printf("what is the farr[fd].startf %d\n",farr[fd].startf);
				LBAread(farr[fd].fbuffer, 1, farr[fd].startf);
//printf("what in buffer?\n %s \n",buffer);
//printf("what is sblk %ld\n",vcbp->sblk);
				vcbp->sblk = savesblk;
				farr[fd].bufferdata = vcbp->sblk;
				farr[fd].fidx = farr[fd].bufferdata;
				
			}
			if(farr[fd].bufferdata <= 0){
					//will return the dataCopied, exiting the function since 
					//EOF is reached, there's nothing to copy to buffer
					
					return dataCopied;
			}
				//the copying of the bufferIndex data into the buffer
				//setting the space at the index of the buffer to what is in the bufferIndex's index
				buffer[dataCopied] = farr[fd].fbuffer[farr[fd].fidx - farr[fd].bufferdata];

				//decrementing the bufferData so that we know where we are in the bufferIndex.
				farr[fd].bufferdata--;

				//incrementing the dataCopied so we know how much we copied over into the buffer
				dataCopied++;
				
		}
printf("what is bufferdata %d\n",farr[fd].bufferdata);
		printf("what us datacopied %d\n",dataCopied);
		//return the dataCopied so that in main, they'll know where to add the '\0'
		return dataCopied;
	
		
		
printf("whats' in buffer\n %s\n",buffer);
printf("at end of b_read what's  wrong with block size %ld\n",vcbp->sblk);
	//return trk;
}

int b_write (int fd, char * buffer, int count){
printf("--------inside the b_write in b_io.c------------\n");
printf("start of b_write what's wrong with blockisze %ld\n",vcbp->sblk);
	//check permissions
	if(farr[fd].per == -1){printf("ERROR: file was closed. file not opened\n");}
	if(farr[fd].per == 1){printf("ERROR: only read allowed\n");}
	//if(count == 0){LBAwrite(farr[fd].fbuffer, 1, farr[fd].startf);}
	//int trk = 0;
	//need case for if b_close calls this to write rest to file
	//
printf("after per check of b_write what's wrong with blockisze %ld\n",vcbp->sblk);
	//copy byte by byte :) cries in time complexity
	/*while(trk < count){
		//check if file buffer full
		if(farr[fd].fidx == vcbp->sblk){
printf("inside the if statemt of b_write\n");
	LBAwrite(farr[fd].fbuffer, 1, farr[fd].startf);
printf("wat was the position to write %d\n", farr[fd].startf);
	return 0;
		}*/
		int readNum = 0;		//keep track of bytes written or in this case, copied
	//to write the last block that didn't make it to 512
	if(count == 0){
		uint64_t sav  = vcbp->sblk;
		LBAwrite(farr[fd].fbuffer,1, farr[fd].startf);
		vcbp->sblk = sav;
	}
printf("after count == 0 of b_write what's wrong with blockisze %ld\n",vcbp->sblk);
uint64_t damit = vcbp->sblk;
	//copies the caller's buffer byte by byte until the requested count is reached
	while(readNum < count){
		farr[fd].fbuffer[farr[fd].fidx] = buffer[readNum];
		readNum++;			//update how much read
		farr[fd].fidx++;		//update where we are
		//writing when the buffer is full
		if(farr[fd].fidx == damit){
printf("b4 lbawrite of b_write what's wrong with blockisze %ld\n",vcbp->sblk);
			uint64_t savev = vcbp->sblk;
			LBAwrite(farr[fd].fbuffer, 1, farr[fd].startf);
			vcbp->sblk = savev;
printf("after lbawrite of b_write what's wrong with blockisze %ld\n",vcbp->sblk);
		}

	}	
vcbp->sblk = damit;
printf("end of b_write what's wrong with blksize %ld\n",vcbp->sblk);
	return (readNum);
}

int b_seek (int fd, off_t offset, int whence){
printf("---------inside the b_seek in the b_io.c----------\n");
	//not sure about this since we only used this like once
	return 0;
}

void b_close (int fd){
printf("--------inside the b_close in the b_io.c-------\n");
	if(farr[fd].bufferdata > 0){b_write(fd, farr[fd].fbuffer, 0);}
	//fiop->per = -1; //???? not sure
	//release any unused space?
	//free(fiop);
}
