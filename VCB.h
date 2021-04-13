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

void init_VCB_blk(uint64_t  nblk, uint64_t  sblk);
uint64_t init_free_space();
int allocate_free_space(int nblksn);
void set_free_space(int LBA, int count);
void release_free_space(int LBA, int count);
void allocate_dir();
uint64_t createDir(char name [],uint64_t i);

#define Signu 0x726f766976727553

#define H_CAPTION "Please save me. I need helppppppppppppppppppppppppppp\n\n"
