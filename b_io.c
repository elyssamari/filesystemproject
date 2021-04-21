#include "b_io.h"
#include <stdio.h>
#include "VCB.h"
#include <stdlib.h>
#include <string.h>
typedef struct finfo{
	int bc;			//blockcount of file
	int fidx;		//file index
	char * fbuffer;		//file buffer
	int per;		//file permission
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
	fiop -> fbuffer = 0;
//printf("what is the value of length of dea %d\n",s);
	//how to get VCB dea in this file tho. not sure
	//of cousre it won't go up to 10, gotta find length
	for(int i = 0; i<10; i++){
		if(strcmp(dea[i].dename,filename)==0){
printf("did we make it into the if statement for comparing filename\n");
			return dea[i].loc;
		}
	}
	
	//if reach here, then there is no file. gotta make it
	return 0;
}

int b_read (int fd, char * buffer, int count){
printf("-----------inside the b_read in b_io.c------------\n");
	if(fiop->per == -1){printf("ERROR: file not opened\n");}
	//keep track of how much is copied
	int trk = 0;
	
	//loop to copy
	while(trk<count){
		//read to the file buffer for one block at given fd
		//this should happen once
		if(fiop->fidx == 0 && fiop->bc == 0){
			LBAread(fiop->fbuffer,1,fd);
		}

		//copy into caller buffer
		buffer[trk] = fiop->fbuffer[fiop->fidx];
		fiop->fidx++;			
		trk++;				//update the two trackers

		//check if we reached end of block
		if(fiop->fidx > vcbp->sblk){
			fiop->bc++;			//update blocks read
			fiop->fidx = 0; 		//reset tracker
			LBAread(fiop->fbuffer, 1, fd + fiop->bc);
		}
	}

	return trk;
return 0;
}

int b_write (int fd, char * buffer, int count){
printf("--------inside the b_write in b_io.c------------\n");
	/*//check permissions
	if(fiop->per == -1){printf("ERROR: file not opened\n");}
	if(fiop->per == 4){printf("ERROR: only read allowed\n");}

	//check if we can write to the block at fd
	if(count > blksize*blkcountoffdfile){gotta allocate more space;}
	//or
	if(count <= what's left of the free block){
		LBAwrite(buffer,1,fd);
	}*/
	return 0;
}

int b_seek (int fd, off_t offset, int whence){
printf("---------inside the b_seek in the b_io.c----------\n");
	//not sure about this since we only used this like once
	return 0;
}

void b_close (int fd){
printf("--------inside the b_close in the b_io.c-------\n");
	fiop->per = -1; //????
	//closPartitionSytem()?

}




