#include "mfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
VCB_p vcbp;
char*curdir;
//what is mode?
int fs_mkdir(const char *pathname, mode_t mode){
printf("----------------insdie the fs_mkdir in mfs.h----------------\n");

printf("what is that pathname %s\n",pathname);
	char* hello = malloc(8);
	strcpy(hello,pathname);
//printf("what is the sblk here %ld\n",vcbp->sblk);
//printf("what is going on here?\n why do i get seg faullts\n");
//printf("after strcopy in fs_mkdir\n");
	int wh = allocate_free_space(2);
//printf("wherewill it o to %d\n",wh);
	int sucs = makede(hello,wh,2,1);
//printf("after the makede in fs_mkdir\n");
	if(sucs > 0 ){return sucs;}
	return -1;
}

//uhhhhh
int fs_rmdir(const char *pathname){
	//release?
	//set dea values to 0?
	for(int i = 0; i<vcbp->sroots; i++){

		if(strcmp(pathname,dea[i].dename)==0){
			release_free_space(dea[i].loc, dea[i].size);
			dea[i].ford = 0;
			dea[i].size = 0;
			dea[i].loc = 0;
			strcpy(dea[i].dename,"        ");
			LBAwrite(dea, vcbp->srootbs, vcbp->sroot);
		}
	}
	return 0;
}

//am i doing this right?
fdDir * fs_opendir(const char *name){
printf("---------------inside the fs_opendir----------------\n");
//printf("what is vcbp in fs_opendir %ld\n",vcbp->sroots);
	fdDir * fdr = malloc(sizeof(fdDir));
	for(int i = 0; i<vcbp->sroots; i++){

		if(strcmp(name,dea[i].dename)==0){
			
			fdr->d_reclen = dea[i].size;
			fdr->dirEntryPosition = i;
			fdr->directoryStartLocation = dea[i].loc;
			fdr->gup = 0;
		}
	}
	
	return fdr;

}

//am I doing this right?
struct fs_diriteminfo *fs_readdir(fdDir *dirp){
	struct fs_diriteminfo * dinfo = malloc(sizeof(struct fs_diriteminfo));
	dinfo -> d_reclen = dirp -> d_reclen;
	dinfo -> fileType = 'd';
	strcpy(dinfo -> d_name, dea[(dirp->dirEntryPosition)+dirp->gup].dename);
//printf("what is %s\n",dea[(dirp->dirEntryPosition)].dename);
//printf("what is %s\n",dea[(dirp->dirEntryPosition)+dirp->gup].dename);
if(strcmp(dea[(dirp->dirEntryPosition)+dirp->gup].dename,"")==0){
//printf("its nothing in the readdir\n");
dinfo = NULL;
}
dirp->gup++;


	//dinfo = NULL;
	return dinfo;
}

//maybe?
int fs_closedir(fdDir *dirp){
	free(dirp);
	dirp = NULL;
	return 0;
}

char * fs_getcwd(char *buf, size_t size){
	printf("inside fs_getcwd\n");
	printf("what is curdir %s\n",curdir);
	//if(strcmp(buf,"")==0){buf = ".";}
	//strcpy(curdir,buf);
	return curdir;
}

int fs_setcwd(char *buf){   //linux chdir
	printf("inside fs_setcwd\n");
	printf("what is buf in fs_setcwd %s\n",buf);
	//printf("what is FT_LINK in fs_setcwd %d\n",FT_LINK);
	//printf("what is FT_REGFILE in fs_setcwd %d\n",FT_REGFILE);
	//printf("what is FT_DIRECTORY in fs_setcwd %d\n",FT_DIRECTORY);
	//curdir = buf;
	strcpy(curdir,buf);
	printf("what is curdir %s\n",curdir);
	for(int i = 0; i<vcbp->sroots; i++){

		if(strcmp(buf,dea[i].dename)==0){
			return 0;
		}
	}
return -1;
}

//prob good
int fs_isFile(char * path){	//return 1 if file, 0 otherwise
printf("----------------inside the fs_isFile in mfs.c----------------\n");
printf("what is vcbp in fs_isFile %ld\n",vcbp->sroots);
	for(int i = 0; i < vcbp->sroots; i++){
		if(strcmp(dea[i].dename, path) == 0){
			if(dea[i].ford == 0){
				return 1;
			}
		}
	}
	return 0;
}

//prob good
int fs_isDir(char * path){		//return 1 if directory, 0 otherwise
printf("----------------inside the fs_isDir in mfs.c----------------\n");
printf("what is vcbp in fs_isDir %ld\n",vcbp->sroots);
	for(int i = 0; i < vcbp->sroots; i++){
		if(strcmp(dea[i].dename, path) == 0){
			if(dea[i].ford == 1){
				return 1;
			}
		}
	}
	return 0;
}

int fs_delete(char* filename){	//removes a file
	//release
	for(int i = 0; i<vcbp->sroots; i++){

		if(strcmp(filename,dea[i].dename)==0){
			release_free_space(dea[i].loc, dea[i].size);
			dea[i].ford = 0;
			dea[i].size = 0;
			dea[i].loc = 0;
			strcpy(dea[i].dename,"        ");
			LBAwrite(dea, vcbp->srootbs, vcbp->sroot);
		}
	}
	return 0;
}
int fs_stat(const char *path, struct fs_stat *buf){

	return 0;
}
