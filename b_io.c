/**************************************************************
* Class:  CSC-415-01 Spring 2021
* Name: Annie Liao, Vivian Kuang, Elyssa Tapawan, Joanne Wong
* Student ID: 918266744, 918403595, 918459248, 918441685
* Project: Basic File System
*
* File: b_io.c
*
* Description: The b_io.h functions implemented. 
*
**************************************************************/

#include "b_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mfs.h"
#define maxf 20

typedef struct finfo{
	int blk_count;		//blockcount of file use for r/w
	int fidx;		//file index use for r/w
	char * fbuffer;		//file buffer use for r/w
	int bufferdata;		//how much in buffer
	int permission;		//file permission
	int startf;		//what is the start/lba index
	int file_length;	//blk len of file from that lba index
	int didx;		//index in the de array
	int blkused;		//how much blocks used 
} finfo;

int var = 0; 
finfo farr [maxf];
int started = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
VCB_p vcbp;

void b_init (){
	//init farr to all free
	for (int i = 0; i < maxf; i++){
		farr[i].didx = -1;	//indicates a free farr space
	}

	started = 1;
}

int b_getfarr (){
	for (int i = 0; i < maxf; i++){
		if (farr[i].didx == -1){
			farr[i].didx = -2;	// used but not assigned
			return i;		
		}
	}
	return (-1);  //all in use
}

int b_open (char * filename, int flags){

	if (var == 1){
		printf("Var is 1. Sroots: %ld\n",vcbp->sroots);
	}

	//need to check if the array is init
	if (started == 0){
		b_init();
	}

	//need to get index of free space
	int index = b_getfarr();
	//out of space
	if (index == -1){
		printf("No more space.\n"); 
		return -1;
	}

	//need to find filename in directory array
	for (int i = 0; i < vcbp->sroots; i++){
		if (strcmp(dea[i].dename, filename) == 0){
			farr[index].didx = i;
			break;
		}

		if((i+1)  == vcbp->sroots){
			farr[index].startf = allocate_free_space(3);
			//made currentDir in mfs.h an extern char [] and just cast (char*)
			//to be passed into makede
			if(farr[index].startf == -1){
				return -1;
			}
			farr[index].didx = makede(filename,farr[index].startf,3,0, (char*)currentDir);
		}
	}

	//need to initalize values from struct
	farr[index].file_length = dea[farr[index].didx].size;
	farr[index].startf = dea[farr[index].didx].loc;
	farr[index].blk_count = 0;
	farr[index].fidx = 0;
	farr[index].fbuffer = malloc(vcbp->sblk);
	farr[index].bufferdata = 0;
	//need to determine permissions
	
	if (flags = 0){
	   farr[index].permission = 1;
	}

	if(flags > 0){
	farr[index].permission = 2;
	}

	//need to return farr index
	return index;
}

int b_read (int fd, char * buffer, int count){
	if (farr[fd].permission == -1){
	   printf("ERROR: File was closed. File not opened\n"); 
	   return 0;
	}
	
	//keep track of how much data is copied to the buffer
	int dataCopied = 0;

	//loop makes sure that enough chars are being copied into the buffer, it will 			
	//stop once the requested amount is filled or EOF
	while(dataCopied < count){
	//checking the amount in the file buffer, will execute a read to fill 	
	//file buffer if it's 0
		if (farr[fd].bufferdata == 0 && farr[fd].blk_count<farr[fd].file_length){
		   uint64_t savesblk = vcbp->sblk;
		   LBAread(farr[fd].fbuffer, 1, farr[fd].startf+farr[fd].blk_count);
		   farr[fd].blk_count++;
		   vcbp->sblk = savesblk;
		   farr[fd].bufferdata = vcbp->sblk;
		   farr[fd].fidx = farr[fd].bufferdata;

		}
		if(farr[fd].blk_count == farr[fd].file_length && farr[fd].bufferdata ==0){
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

	//return the dataCopied so that in main, they'll know where to add the '\0'
	return dataCopied;
}

int b_write (int fd, char * buffer, int count){
	//check permissions
	if (farr[fd].permission == -1){
		printf("ERROR: File was closed. File not opened.\n");
	}
	
	if (farr[fd].permission == 1){
		printf("ERROR: Only read allowed.\n");
	}

	int readNum = 0;	//keep track of bytes written or in this case, copied
				//to write the last block that didn't make it to 512
	if (count == 0){
	   uint64_t sav  = vcbp->sblk;
	   while(farr[fd].bufferdata<sav){
		farr[fd].fbuffer[farr[fd].bufferdata]=0;
		farr[fd].bufferdata++;
	   }
	   LBAwrite(farr[fd].fbuffer,1, farr[fd].startf+farr[fd].blk_count);
	   farr[fd].blk_count ++;
	   vcbp->sblk = sav;
	}

	uint64_t damit = vcbp->sblk;
	//copies the caller's buffer byte by byte until the requested count is reached

	while (readNum < count){
	      farr[fd].fbuffer[farr[fd].bufferdata] = buffer[readNum];
	      readNum++;	//update how much read
	      farr[fd].bufferdata++;	//update where we are
	      //writing when the buffer is full

	      if (farr[fd].bufferdata == damit){
			//check if theres enought blocks to write to
			if(farr[fd].file_length <= farr[fd].blk_count){
				//get more blocks if needed
				int newstart = allocate_free_space(2*(farr[fd].file_length));
				if(newstart == -1){
					printf("No more space.\n");
					return -1;
				}
				if(newstart == (farr[fd].startf+farr[fd].blk_count)){
					farr[fd].file_length = farr[fd].file_length + (2*(farr[fd].file_length));
					dea[farr[fd].didx].size = farr[fd].file_length;
					LBAwrite(dea,10,vcbp->sroot);
				}
			}
			uint64_t savev = vcbp->sblk;
			LBAwrite(farr[fd].fbuffer, 1, farr[fd].startf+farr[fd].blk_count);
			farr[fd].blk_count++;
			farr[fd].bufferdata=0;
			vcbp->sblk = savev;
	 	}
	}

	vcbp->sblk = damit;
	return (readNum);
}

int b_seek (int fd, off_t offset, int whence){
	if (whence == SEEK_SET) {
		printf("SEEK_SET: farr->fidx: %d\n", farr[fd].fidx);
		farr[fd].fidx = offset;
		printf("END of SEEK_SET: farr->fidx: %d\n", farr[fd].fidx);
	} else if (whence == SEEK_CUR) {
		printf("SEEK_CUR: farr->fidx: %d\n", farr[fd].fidx);
		farr[fd].fidx += offset;
		printf("END of SEEK_CUR: farr->fidx: %d\n", farr[fd].fidx);
	} else if (whence == SEEK_END) {
		printf("SEEK_END: farr->fidx: %d\n", farr->fidx);
		//get sizeof file cuz we need end of file ? and since .size 
		//is the number of blocks for that file, multiply by 512?
		farr[fd].fidx = dea[farr[fd].didx].size*vcbp->sblk;
		farr[fd].fidx += offset;
		printf("END of SEEK_END: fiop->fidx: %d\n", farr->fidx);
	}
	printf("farr[fd].fidx: %d\n", farr[fd].fidx);
	//not sure what to return? lseek returns the resulting offset location
	//as measured in bytes from the beginning of the file
	return farr[fd].fidx;
}

void b_close (int fd){
	//check if there data left to write
	if (farr[fd].bufferdata > 0){
		b_write(fd, farr[fd].fbuffer, 0);
	}

	//check if theres blocks to be freed
	if(farr[fd].blk_count < farr[fd].file_length){
		release_free_space(farr[fd].blk_count + farr[fd].startf, farr[fd].file_length-farr[fd].blk_count);
		dea[farr[fd].didx].size = farr[fd].blk_count;
		LBAwrite(dea,10,vcbp->sroot);
	}

	free(farr[fd].fbuffer);
	farr[fd].fbuffer = NULL; 
}
