#include "b_io.h"
#include <stdio.h>
#include "VCB.h"
#include <stdlib.h>
#include <string.h>
#include "mfs.h"
typedef struct finfo{
	int bc;			//blockcount of file use for r/w
	int fidx;		//file index use for r/w
	char * fbuffer;		//file buffer use for r/w
	int per;		//file permission
	int startf;		//what is the start/lba index
	int flen;		//blk len of file from that lba index
	int didx;		//index in the de array
}finfo_t, *finfo_p;
int party = 0;
finfo_p fiop = NULL;
fdDir *fdDiri = NULL;

int b_open (char * filename, int flags){
printf("------------------inside the b_open in b_io.c-----------------\n");
printf("did name make it %s\n",filename);
char*testthe = "testone";
if(party == 0 ){
char*bsize = "500000";
char*blksze = "512";
uint64_t bytesize = atoll(bsize);
uint64_t blockysize = atoll(blksze);
printf("------what's wrog with the blksize-----------\n %ld\n",blockysize);
startPartitionSystem(testthe,&bytesize,&blockysize);
init_VCB_blk(bytesize,blockysize);
party = 1;}
	fiop = malloc(1000);
	fdDiri = malloc(1000);
	//set permissions. there's only two that's used in the shell so there's 2 not 7.
	if(flags==0){
		fiop -> per = 1;
	}else if(flags>0){
		fiop -> per=2;
	}
	
	printf("b-io.c b-open filename %s, numof flags %d\n",filename, flags);
	printf("what is the value of vcb here in b_io.c?: %ld\n",vcbp->sroot);
	//printf("what is the value of dea here in b_io.c?: %s\n",dea[2].dename);
	fiop -> bc = 0;
	fiop -> fidx = 0;
	fiop -> fbuffer = malloc(vcbp->sblk);
printf("where did it seg fault? \n");
	//how to get VCB dea in this file tho. not sure
	//of cousre it won't go up to 10, gotta find length. new var?
	for(int i = 0; i<vcbp->sroots; i++){
		if(strcmp(dea[i].dename,filename)==0){
printf("did we make it into the if statement for comparing filename\n");
			fiop->flen = dea[i].size;
			fiop->startf = dea[i].loc;
			fiop->didx = i;
			fdDiri->directoryStartLocation = dea[i].loc;
			fdDiri->dirEntryPosition = i;
			fdDiri->d_reclen = dea[i].size;
			return dea[i].loc;
		}
	}
	
	//if reach here, then there is no file. gotta make it
	//allocate the space to do so
	fiop->startf = allocate_free_space(5);
	//check if there was enough space
	if(fiop->startf == 0){printf("No more space for file.\n");return -1;}
	//create the de
	
	makede(filename,fiop->startf,2);
printf("what is the start of create file %d\n",fiop->startf);
printf("what's wrong with the blkize %ld\n",vcbp->sblk);
	fiop->flen = 2;
	//return the start of the de
	return fiop->startf;
}

int b_read (int fd, char * buffer, int count){
printf("-----------inside the b_read in b_io.c------------\n");
printf("what' swrong with blksize %ld\n",vcbp->sblk);
	if(fiop->per == -1){printf("ERROR: file was closed. file not opened\n");}
	//keep track of how much is copied
	int trk = 0;
printf("made it past per check, b4 the while loop\n");
printf("what is fidx %d\n",fiop->fidx);
	//loop to copy byte by byte :) 
	while(trk<count){
if(fiop->fidx == 512){printf("what is block size %ld\n",vcbp->sblk);fiop->fidx = 0;return 0;}
//printf("inside the while loop. what is trk: %d\n",trk);
		//read to the file buffer for one block at given fd
		if(fiop->fidx == 0){
//printf("after the check fidx. where is seg fault?\n");
printf("b4 the lbaread what is blksize %ld\n",vcbp->sblk);
//i don't know how lbaread changed the vcbp->sblk
//so i save and rest the value
uint64_t idkhowsblk = vcbp->sblk;
			uint64_t rev = LBAread(fiop->fbuffer,1,fd);
printf("after the lbaread what is the return %ld\n",rev);
vcbp->sblk = idkhowsblk;
		//if reached end of blk read again
		}/*else if(fiop->fidx >= vcbp->sblk){
printf("+++++++++++++what is fidx %d and bc %d and flen %d\n",fiop->fidx, fiop->bc,fiop->flen);
			fiop->bc++;			//update blocks read
			fiop->fidx = 0; 		//reset tracker
			//check if any more valid blks to read
			//if not, return 0
			if(fiop->bc == fiop->flen){fiop->bc=0;return 0;}
			//if yes read
			LBAread(fiop->fbuffer, 1, fd + fiop->bc);
			
		}*/

		//copy into caller buffer
		buffer[trk] = fiop->fbuffer[fiop->fidx];
		fiop->fidx++;			
		trk++;				//update the two trackers

		//check if we reached end of block
		/*if(fiop->fidx > vcbp->sblk){
			fiop->bc++;			//update blocks read
			fiop->fidx = 0; 		//reset tracker
			LBAread(fiop->fbuffer, 1, fd + fiop->bc);
		}*/
	}
printf("what is trk %d\n",trk);
printf("whats' in buffer\n %s\n",buffer);
printf("at end of b_read what's  wrong with block size %ld\n",vcbp->sblk);
	return trk;
}

int b_write (int fd, char * buffer, int count){
printf("--------inside the b_write in b_io.c------------\n");
printf("start of b_write what's wrong with blockisze %ld\n",vcbp->sblk);
	//check permissions
	if(fiop->per == -1){printf("ERROR: file was closed. file not opened\n");}
	if(fiop->per == 4){printf("ERROR: only read allowed\n");}
	if(count == 0){LBAwrite(fiop->fbuffer,1,fiop->startf);}
	int trk = 0;
	//need case for if b_close calls this to write rest to file
	//

	//copy byte by byte :) cries in time complexity
	while(trk < count){
		//check if file buffer full
		if(fiop->fidx == vcbp->sblk){
printf("inside the if statemt of b_write\n");
	LBAwrite(fiop->fbuffer, 1, fiop->startf);
printf("wat was the position to write %d\n",fiop->startf);
	return 0;
			//check there even is a blk to write to
			//if(fiop->bc < fiop->flen){
	
			//	LBAwrite(fiop->fbuffer,1,fd+fiop->bc);
			//	fiop->bc++;
			/*}else{
				//hee hee now that's an issue ain't it
				//gotta find free space to meet needs
				int c = allocate_free_space(fiop->flen+fiop->flen);
				if(c==0){printf("no more space\n");return 0;}
				//copy info there
				int ctrk = 0;
				char * cbuffer = malloc(vcbp->sblk);
				//copying what's in the blocks
				while(ctrk < fiop->flen){
					LBAread(cbuffer,1,ctrk+fiop->startf);
					LBAwrite(cbuffer, 1, ctrk+c);
					ctrk++;
				}

				//write the block that caused this 
				LBAwrite(fiop->fbuffer,1,ctrk+c);
				//resetting values
				fiop->startf = c;
				dea[fiop->didx].loc = c;
				dea[fiop->didx].size = fiop->flen + fiop->flen;
				fiop->flen = dea[fiop->didx].size;
				
			}*/

		}
		//copy caller buffer to file buffer
		fiop->fbuffer[fiop->fidx] = buffer[trk];
		fiop->fidx++;				//update the trackers
		trk++;
	}

	/*//check if we can write to the block at fd
	if(count >= blksize){gotta allocate more space;}
	//or
	if(count <= what's left of the free block){
		LBAwrite(buffer,1,fd);
	}*/
printf("end of b_write what's wrong with blksize %ld\n",vcbp->sblk);
	return trk;
}

int b_seek (int fd, off_t offset, int whence){
printf("---------inside the b_seek in the b_io.c----------\n");
	//not sure about this since we only used this like once
	return 0;
}

void b_close (int fd){
printf("--------inside the b_close in the b_io.c-------\n");
	if(fiop->fidx > 0){b_write(fd, fiop->fbuffer, 0);}
	fiop->per = -1; //???? not sure
	//release any unused space?
	//free(fiop);
}


