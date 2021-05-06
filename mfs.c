#include "mfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
VCB_p vcbp;
char currentDir[100]=".";
int count = 0;
int c = 0;
int cnt = 0;
fdDir * fdr;
struct fs_diriteminfo * dinfo;

int fs_mkdir(const char *pathname, mode_t mode){
	printf("----------------inside the fs_mkdir in mfs.h----------------\n");
	printf("Pathname: %s\n",pathname);
	
	for(int i = 0; i<vcbp->sroots; i++){
		if (strcmp(currentDir,dea[i].currentDir)==0 && strcmp(pathname, dea[i].dename)==0){
			printf("Directory name already exists in this current working directory.\n");
			return -1;
		}
	}

	char* pname = malloc(10);
	strcpy(pname,pathname);
	//int free_space = allocate_free_space(2); //was wh
	int entry = makede(pname,0,0,1,currentDir);	
	if(entry > 0 ){
		//can free here?
		free(pname);
		pname = NULL;
		return entry;
	}

	return -1;
}


int fs_rmdir(const char *pathname){
printf("----------inside the fs_rmdir in mfs.c ---------------");
      	char * str = (char *) malloc(2 + strlen(currentDir)+ strlen(pathname));
	char rmDir1[100];
	char rmDir2[100];
  	strcpy(str, currentDir);
  	strcat(str, "/");
      	strcat(str, pathname);
	strcpy(rmDir1, str);
  	strcat(str, "/");
	strcpy(rmDir2, str);

	for(int i = 0; i<vcbp->sroots; i++){
		char cdir2[100];
		strncpy(cdir2, dea[i].currentDir, strlen(rmDir2));

		if ((strcmp(rmDir1,cdir2)==0) || (strcmp(rmDir2,cdir2)==0) ||
			(strcmp(currentDir, dea[i].currentDir)==0 && strcmp(pathname, dea[i].dename)==0)){
		   release_free_space(dea[i].loc, dea[i].size);
		   dea[i].ford = 0;
		   dea[i].size = 0;
		   dea[i].loc = 0;
		   dea[i].dename[0]='\0';
		   dea[i].currentDir[0]='\0';
		   LBAwrite(dea, vcbp->srootbs, vcbp->sroot);

		}
	}

	free(str);
	str = NULL;

	return 0;
}

fdDir * fs_opendir(const char *name){
	printf("---------------inside the fs_opendir----------------\n");
	//need to free this somewhere because its malloc-ed;
	//i make it global and freed in rmdir but...?
    	fdr = malloc(sizeof(fdDir));
    
	for(int i = 0; i<vcbp->sroots; i++){
        	if (strcmp(currentDir,dea[i].currentDir)==0){
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


struct fs_diriteminfo *fs_readdir(fdDir *dirp){
	//need to free this somewhere because its malloc-ed?
	//i made global and freed in rmdir but...?
	dinfo = malloc(sizeof(struct fs_diriteminfo));
	dinfo -> d_reclen = dirp -> d_reclen;
	dinfo -> fileType = 'd';
	strcpy(dinfo -> d_name, dea[dirp->location[cnt]].dename);
   
	if (count < cnt && strcmp(currentDir,".")==0){
		dinfo = NULL;
		count =0;
		cnt = 0;
    	} else if (count <= cnt && strcmp(currentDir,".")!=0) {
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
	return currentDir;
}

int fs_setcwd(char *buf){   
	int retVal = -1;
	char path[100];
	char *parameters[100];
	int count = 0;
	if (strcmp(buf,"..")==0 && strcmp(currentDir,".")!=0){
		char * split = strtok(currentDir, "/");
		while (split != NULL) {
			parameters[count++] = strdup(split);
			split = strtok(NULL, "/");
		}

		for (int i = 0; i != count-1; i++) {
			if (i==0){
				strcpy(path, parameters[i]);
			}
			if(i!=0 && count != 2 && i!=count-1){
  				strcat(path, "/");
				strcat(path, parameters[i]);
			}

			
		}
		strcpy(currentDir, path);
		return 0;
	}

	if (strcmp(buf,".")==0){
		return 0;
	}

	for(int i = 0; i<vcbp->sroots; i++){
		if(strcmp(currentDir,dea[i].currentDir)==0 && strcmp(buf, dea[i].dename)==0){
			retVal = 0;
		}
	}

	if (retVal == 0){
      		char * str3 = (char *) malloc(2 + strlen(currentDir)+ strlen(buf) );
  		strcpy(str3, currentDir);
  		strcat(str3, "/");
      		strcat(str3, buf);
		strcpy(currentDir, str3);
		//should free str3? 
		free(str3);
		str3 = NULL;
		return 0;
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
printf("--------inside the fs_delete in mfs.c--------------");
	for(int i = 0; i<vcbp->sroots; i++){
		if (strcmp(filename,dea[i].dename)==0){
		   release_free_space(dea[i].loc, dea[i].size);
		   dea[i].ford = 0;
		   dea[i].size = -1;
		   dea[i].loc = 0;
		   strcpy(dea[i].dename,"        ");
		   LBAwrite(dea, vcbp->srootbs, vcbp->sroot);
		   return 0;
		}
	}

	return -1;
}

int fs_stat(const char *path, struct fs_stat *buf){
//create time_t variable to grab the time needed to store values into the struct
	time_t rawTime;
	//loop through to find the proper directory entry
	for (int i = 0; i < vcbp->sroots; i++) {
		if (strcmp(dea[i].dename, path) == 0) {
			//total size in bytes; since .size = num of blocks allocated to it i assume
			buf->st_size = dea[i].size*vcbp->sblk;
			printf("buf size: %li\n", buf->st_size);
			//block size of file system I/O; i assumed it was asking for how big 
			//each block size was? thats why i set as sblk from vcbp
			buf->st_blksize = vcbp->sblk;
			printf("buf blksize: %li\n", buf->st_blksize);
			//assuming size in de struct is bumber of blocks, set as such
			buf->st_blocks = dea[i].size;
			printf("buf blocks: %li\n", buf->st_blocks);

			//how to get a time_t that doesn't update with the computer's time / date
			//for createtime

			//finds current time + saves it to acesstime variable
			time(&rawTime);
			buf->st_accesstime = rawTime;
			//rawTime itself isn't readable by itself; ctime to make it readable
			printf("file: %s\n", dea[i].dename);
			printf("time: %s\n", ctime(&buf->st_accesstime)); 
			
			//modtime would need to check when the file was changed but how? hm
		 } 
	}
	return 0;
}
