/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: VCB.h
*
* Description: this is the header file of the VCB.c.
*		I didn't know how to share values between
*		different c files. I had to google it and
*		learned about extern from stackoverflow. I'm
*		sorry.
**************************************************************/
#include "fsLow.h"
#define Signu 0x726f766976727553
#define H_CAPTION "Message Test: Hello World this is the group Survivors\n\n"	//Message Test

typedef struct VCB{
	char VCBPrefix[sizeof(H_CAPTION)+2];	//Message Test
	uint64_t mnum;				//magic number maybe forgot what it was
	uint64_t signu;				//signature forgot what it was
	uint64_t nblkt;				//words for num of blocks to see in hexdump
	uint64_t nblk;				//number of blocks
	uint64_t sblkt;				//words for size of blocks to see in hexdump
	uint64_t sblk;				//size of block
	uint64_t sroott;			//words for root dir to see in hexdump
	uint64_t sroot;				//where does root dir start
	uint64_t sroots;			//how many possible entries
	uint64_t srootbs;			//blosk size of rootdir
	uint64_t sfst;				//words for free space to see in hexdump
	uint64_t sfs;				//where does free space start
	uint64_t sffst;				//words for free space size
	uint64_t sffs;				//block size of free space map
} VCB_t, *VCB_p;

//based on the struct provided in the Tuesday lecture 4/13
typedef struct de{
	uint64_t ford;				//file (0) or directory (1)
	uint64_t size;				//size of de
	uint64_t loc;				//where is de
	uint64_t namet;				//to see name on hexdump
	char  dename [10];			//name of directory
	char currentDir[20];			//current working directory 
						//where the directory was made (pwd)
} de_t, *de_p;

/*typedef struct ftbl{
	uint64_t begins;
	uint64_t length;
}ftbl_t,*ftbl_p;*/

//#include "fsLow.h"
extern VCB_p vcbp;								//VCB_p vcbp = NULL in VCB.c
extern de_t *dea;								//de_t * dea = NULL in VCB.c
int init_VCB_blk(uint64_t  nblk, uint64_t  sblk);				//init_VCB_blk in VCB.c
uint64_t init_free_space();							//init_free_space in VCB.c
int allocate_free_space(int nblksn);						//allocate_free_space in VCB.c
void set_free_space(int LBA, int count);					//set_free_space in VCB.c
void release_free_space(int LBA, int count);					//release_free_space in VCB.c
void allocate_dir();								
uint64_t makede(char*fname, uint64_t index,uint64_t sz,uint64_t fod, char*currentDir);	//makede in VCB.c
uint64_t createDir(char name [],uint64_t i);					//createDir in VCB.c
int getval(); 									//getval in VCB.c
void close_vcb();
