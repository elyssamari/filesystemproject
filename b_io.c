#include "b_io.h"
#include <stdio.h>
#include "VCB.h"
#include <stdlib.h>
#include <string.h>
typedef struct finfo{
	int bc;			//blockcount of file use for r/w
	int fidx;		//file index use for r/w
	char * fbuffer;		//file buffer use for r/w
	int per;		//file permission
	int startf;		//what is the start/lba index
	int flen;		//blk len of file from that lba index
	int didx;		//index in the de array
}finfo_t, *finfo_p;

finfo_p fiop = NULL;

int b_open (char * filename, int flags){
printf("------------------inside the b_open in b_io.c-----------------\n");
printf("did name make it %s\n",filename);
	fiop = malloc(1000);
	//set permissions. there's only two that's used in the shell so there's 2 not 7.
	if(flags==1){
		fiop -> per = 1;
	}else if(flags>1){
		fiop -> per=2;
	}
	
	printf("b-io.c b-open filename %s, numof flags %d\n",filename, flags);
	printf("what is the value of vcb here in b_io.c?: %ld\n",vcbp->sroot);
	printf("what is the value of dea here in b_io.c?: %s\n",dea[2].dename);
	fiop -> bc = 0;
	fiop -> fidx = 0;
	fiop -> fbuffer = malloc(vcbp->sblk);
//printf("what is the value of length of dea %d\n",s);
	//how to get VCB dea in this file tho. not sure
	//of cousre it won't go up to 10, gotta find length. new var?
	for(int i = 0; i<vcbp->sroots; i++){
		if(strcmp(dea[i].dename,filename)==0){
printf("did we make it into the if statement for comparing filename\n");
			fiop->flen = dea[i].size;
			fiop->startf = dea[i].loc;
			fiop->didx = i;
			return dea[i].loc;
		}
	}
	
	//if reach here, then there is no file. gotta make it
	//allocate the space to do so
	fiop->startf = allocate_free_space(5);
	//check if there was enough space
	if(fiop->startf == 0){printf("No more space for file.\n");return -1;}
	//create the de
	
	makede(filename,fiop->startf,5);
	

	fiop->flen = 5;
	//return the start of the de
	return fiop->startf;
}

int b_read (int fd, char * buffer, int count){
printf("-----------inside the b_read in b_io.c------------\n");
	if(fiop->per == -1){printf("ERROR: file was closed. file not opened\n");}
	//keep track of how much is copied
	int trk = 0;
	
	//loop to copy byte by byte :) 
	while(trk<count){
		//read to the file buffer for one block at given fd
		if(fiop->fidx == 0){
			LBAread(fiop->fbuffer,1,fd);
		//if reached end of blk read again
		}else if(fiop->fidx >= vcbp->sblk){
			
			fiop->bc++;			//update blocks read
			fiop->fidx = 0; 		//reset tracker
			//check if any more valid blks to read
			//if not, return 0
			if(fiop->bc == fiop->flen){fiop->bc=0;return 0;}
			//if yes read
			LBAread(fiop->fbuffer, 1, fd + fiop->bc);
			
		} 

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

	return trk;
}

int b_write (int fd, char * buffer, int count){
printf("--------inside the b_write in b_io.c------------\n");
	//check permissions
	if(fiop->per == -1){printf("ERROR: file was closed. file not opened\n");return 0;}
	if(fiop->per == 4){printf("ERROR: only read allowed\n");return 0;}
	
	int trk = 0;
	//need case for if b_close calls this to write rest to file
	//

	//copy byte by byte :) cries in time complexity
	while(trk < count){
		//check if file buffer full
		if(fiop->fidx == vcbp->sblk){
			//check there even is a blk to write to
			if(fiop->bc < fiop->flen){
	
				LBAwrite(fiop->fbuffer,1,fd+fiop->bc);
				fiop->bc++;
			}else{
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
				
			}

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
	return trk;
}

int b_seek (int fd, off_t offset, int whence){
printf("---------inside the b_seek in the b_io.c----------\n");
	//not sure about this since we only used this like once
	return 0;
}

void b_close (int fd){
printf("--------inside the b_close in the b_io.c-------\n");
	fiop->per = -1; //???? not sure
	//release any unused space?
	free(fiop);
}




