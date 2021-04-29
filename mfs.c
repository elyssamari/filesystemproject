#include "mfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
VCB_p vcbp;
char*curdir; //what is mode?
char currentPath[100]=".";
int count = 0;

int fs_mkdir(const char *pathname, mode_t mode){
	printf("----------------inside the fs_mkdir in mfs.h----------------\n");
	printf("Pathname: %s\n",pathname);
	char* pname = malloc(8);
	strcpy(pname,pathname);
	//printf("what is the sblk here %ld\n",vcbp->sblk);
	//printf("what is going on here?\n why do i get seg faullts\n");
	//printf("after strcopy in fs_mkdir\n");
	int free_space = allocate_free_space(2); //was wh
	//printf("wherewill it o to %d\n",free_space);
	int entry = makede(pname,free_space,2,1,currentPath);
	//printf("after the makede in fs_mkdir\n");
	
	if(entry > 0 ){
	return entry;
	}
	return -1;
}

//Progress: not sure
int fs_rmdir(const char *pathname){
	//release?
	//set dea values to 0?
	for(int i = 0; i<vcbp->sroots; i++){
		if (strcmp(pathname,dea[i].dename)==0){
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

//Progress: not sure
fdDir * fs_opendir(const char *name){
	printf("---------------inside the fs_opendir----------------\n");
    	fdDir * fdr = malloc(sizeof(fdDir));
    
	for(int i = 0; i<vcbp->sroots; i++){
        	if (strcmp(currentPath,dea[i].path)==0){
            	   fdr->d_reclen = dea[i].size;
            	   fdr->dirEntryPosition = i;
            	   fdr->directoryStartLocation = dea[i].loc;
            	   fdr->location[count]= i;
            	   fdr->increment = count;
            	   count++;
        	}
    	}

    return fdr;
}

int cnt = 0;
struct fs_diriteminfo *fs_readdir(fdDir *dirp){
	struct fs_diriteminfo * dinfo = malloc(sizeof(struct fs_diriteminfo));
	dinfo -> d_reclen = dirp -> d_reclen;
	dinfo -> fileType = 'd';
	strcpy(dinfo -> d_name, dea[dirp->location[cnt]].dename);
   
	if (count < cnt && strcmp(currentPath,".")==0){
           dinfo = NULL;
           count =0;
           cnt = 0;
    	} else if (count <= cnt && strcmp(currentPath,".")!=0) {
          dinfo = NULL;
          count =0;
          cnt = 0;
    	} else{
          cnt++;
   	}

    return dinfo;

}

//Progress: not sure
int fs_closedir(fdDir *dirp){
	free(dirp);
	dirp = NULL;
	return 0;
}

char * fs_getcwd(char *buf, size_t size){
	return currentPath;
}

int fs_setcwd(char *buf){   //linux chdir
	int retVal = -1;
    
	for(int i = 0; i<vcbp->sroots; i++){
        	if (strcmp(currentPath,dea[i].path)==0 && strcmp(buf, dea[i].dename)==0){
            	   retVal = 0;
        }
    }

    	if (retVal == 0){
           char * str3 = (char *) malloc(2 + strlen(currentPath)+ strlen(buf) );
           strcpy(str3, currentPath);
           strcat(str3, "/");
           strcat(str3, buf);
           strcpy(currentPath, str3);
        }
    return retVal;
}

//Progress: good
int fs_isFile(char * path){	//return 1 if file, 0 otherwise
	printf("----------------inside the fs_isFile in mfs.c----------------\n");
	printf("Vcbp in fs_isFile: %ld\n",vcbp->sroots);

	for(int i = 0; i < vcbp->sroots; i++){
		if (strcmp(dea[i].dename, path) == 0){
			if (dea[i].ford == 0){
			   return 1;
			}
		}
	}

	return 0;
}

//Progress: good
int fs_isDir(char * path){		//return 1 if directory, 0 otherwise
	printf("----------------inside the fs_isDir in mfs.c----------------\n");
	printf("Vcbp in fs_isDir: %ld\n",vcbp->sroots);
	
	for(int i = 0; i < vcbp->sroots; i++){
		if(strcmp(dea[i].dename, path) == 0){
			if (dea[i].ford == 1){
			   return 1;
			}
		}
	}
	return 0;
}

int fs_delete(char* filename){	//removes a file
	//release
	for(int i = 0; i<vcbp->sroots; i++){
		if (strcmp(filename,dea[i].dename)==0){
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
