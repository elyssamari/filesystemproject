/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: VCB.h
*
* Description: This is a demo to show how to use the fsLow
* 	routines.
*
**************************************************************/
#include "fsLow.h"
#define Signu 0x726f766976727553

#define H_CAPTION "Please save me. I need helppppppppppppppppppppppppppp\n\n"

#define tblk "Hello there, this is a test to test the b_read function in b_io.c. It is 512. The following is randomness. This world is so sad. What are supposed to do? How can we save this world? Why can the rich and powerful do what they want while the rest of us feel the impact of their choices? Why do we have people who deny the facts just because it is against their religion? How are we supposed to move forward when we are so divided? In the end, it will be too late. Never gonna give you up, never gonna let you down."
typedef struct VCB{
	char VCBPrefix[sizeof(H_CAPTION)+2];	//my helpppppp message
	uint64_t mnum;				//magic number maybe forgot what it was
	uint64_t signu;				//signature forgot what it was
	uint64_t nblkt;				//words for num of blocks to see in hexdump
	uint64_t nblk;				//number of blocks
	uint64_t sblkt;				//words for size of blocks to see in hexdump
	uint64_t sblk;				//size of block
	uint64_t sroott;			//words for root dir to see in hexdump
	uint64_t sroot;				//where does root dir start
	uint64_t sroots;			//how many possible entries
	uint64_t sfst;				//words for free space to see in hexdump
	uint64_t sfs;				//where does free space start
	uint64_t sffst;				//words for free space size
	uint64_t sffs;				//size of free space map
	
}VCB_t, *VCB_p;

typedef struct de{
	uint64_t ford;				//file (0) or directory (1)
	uint64_t size;				//size of de
	uint64_t loc;				//where is de
	uint64_t namet;				//to see name on hexdump
	char  dename [8];			//name of de
}de_t, *de_p;
//#include "fsLow.h"
extern VCB_p vcbp;
extern de_t *dea;
void init_VCB_blk(uint64_t  nblk, uint64_t  sblk);
uint64_t init_free_space();
int allocate_free_space(int nblksn);
void set_free_space(int LBA, int count);
void release_free_space(int LBA, int count);
void allocate_dir();
uint64_t makede(char*fname, uint64_t idx,uint64_t sz);
uint64_t createDir(char name [],uint64_t i);

