/**************************************************************
* Class:  CSC-415-01 Spring 2021
* Name: Annie Liao, Vivian Kuang, Elyssa Tapawan, Joanne Wong
* Student ID: 918266744, 918403595, 918459248, 918441685
* Project: Basic File System
*
* File: b_io.h
*
* Description: Interface of basic I/O functions
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>


int b_open (char * filename, int flags);
int b_read (int fd, char * buffer, int count);
int b_write (int fd, char * buffer, int count);
int b_seek (int fd, off_t offset, int whence);
void b_close (int fd);

#endif

