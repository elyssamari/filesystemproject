#include "mfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
VCB_p vcbp;
int fs_mkdir(const char *pathname, mode_t mode){
printf("insdie the fs_mkdir in mfs.h\n");

printf("what is that pathname %s\n",pathname);
	char* hello = malloc(80);
	strcpy(hello,pathname);
printf("what is the sblk here %ld\n",vcbp->sblk);
printf("what is going on here?\n why do i get seg faullts\n");
printf("after strcopy in fs_mkdir\n");
	int wh = allocate_free_space(2);
printf("wherewill it o to %d\n",wh);
	int sucs = makede(hello,wh,2);
printf("after the makede in fs_mkdir\n");
	if(sucs > 0 ){return sucs;}
	return -1;
}
int fs_rmdir(const char *pathname){
	//release
	return 0;
}
fdDir * fs_opendir(const char *name){
printf("inside the fs_opendir\n");
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

int fs_closedir(fdDir *dirp){
	free(dirp);
	return 0;
}

char * fs_getcwd(char *buf, size_t size){
	if(strcmp(buf,"")==0){buf = ".";}
	//buf = "istest";
	return buf;
}

int fs_setcwd(char *buf){   //linux chdir
return 0;
}
int fs_isFile(char * path){	//return 1 if file, 0 otherwise
printf("inside the fs_isFile in mfs.c\n");
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
int fs_isDir(char * path){		//return 1 if directory, 0 otherwise
printf("inside the fs_isDir in mfs.c\n");
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
	return 0;
}
int fs_stat(const char *path, struct fs_stat *buf){
	return 0;
}
