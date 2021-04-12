#include "b_io.h"
#include <stdio.h>

int b_open (char * filename, int flags){
	//search filename via table?
	//return 1st index of that file
}
int b_read (int fd, char * buffer, int count){
	//get the position from VCB
	//assuming it's the index of the fd
	//using int bytesread = LBAread(&buffer,count,position);
	//return bytesread
}
int b_write (int fd, char * buffer, int count){
	//using index and count, write to the freespace of file
	//using int byteswritten = LBAwrite(&buffer,count, postion);
	//return byteswritten
}
int b_seek (int fd, off_t offset, int whence){

}
void b_close (int fd){

}




