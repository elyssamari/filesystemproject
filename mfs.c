#include "mfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
VCB_p vcbp;
char currentDir[100]=".";
int count = 0;
int count2 = 0;
fdDir * fdr;
struct fs_diriteminfo * dinfo;

int fs_mkdir(const char *pathname, mode_t mode){
	printf("Pathname: %s\n",pathname);
	for(int i = 0; i<vcbp->sroots; i++){
		if (strcmp(currentDir,dea[i].currentDir)==0 && strcmp(pathname, dea[i].dename)==0){
			printf("Directory name already exists in this current working directory.\n");
			return -1;
		}
	}

	char* pname = malloc(10);
	strcpy(pname,pathname);
	//int free_space = allocate_free_space(2);
	int entry = makede(pname,0,0,1,currentDir);
	
	if(entry > 0 ){
		free(pname);
		pname = NULL;
		return entry;
	}

	return -1;
}

/* In fs_rmdir, we are removing a directory and inside that directory, there can also be multiple directories
so we also have to remove those directories as well. I made a new char variable called currentDir which stands 
for the current working directory (pwd). Everytime a directory is made, we save the current working directory 
into the directory entry struct. So in order to remove directories within adirectory, I just need to check the 
currentDir variable. For example, our current working directory (pwd) is . and inside ., we have directory newDir 
in which want to remove. Inside newDir, there is newDir1 and inside newDir1, there is newDir2. We want to remove 
all of those. The currentDir for newDir is . and the the currentDir for newDir1 is ./newDir and currentDir for 
newDir2 is ./newDir/newDir1. In order to remove newDir1 and newDir2, I just make 2 substrings ./newDir and ./newDir/
in which I will use to compare all the currentDir saved in every member of the directory entry struct. newDir2
has currentDir as ./newDir/newDir1 but my substring is ./newDir/ so I just need to cut every currentDir and see
if it matches with ./newDir/ and remove it that directory. */
int fs_rmdir(const char *pathname){
	int retVal; 
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
			memset(dea[i].dename, 0, strlen(dea[i].dename));
			memset(dea[i].currentDir, 0, strlen(dea[i].currentDir));
			LBAwrite(dea, vcbp->srootbs, vcbp->sroot);
			retVal = 0;
		}
	}

	free(str);
	str = NULL;

	if (retVal == 0) {
		return 0;
	}

	return -1;
}


/* In fs_opendir, we are going through the directory entry array (struct) to search for directory entries that match
the currentDir name. The currentDir variable means the current working directory (pwd). When a directory or file is
made, we save the current working directory name which the directory was made into a char variable currentDir. This
helps us easily track what are files or directories are currently in the currently working directory in which 
we want to see for the ls command. */
fdDir * fs_opendir(const char *name){
    	fdr = malloc(sizeof(fdDir));
    
	for(int i = 0; i<vcbp->sroots; i++){
		if (strcmp(currentDir,dea[i].currentDir)==0){
			fdr->d_reclen = dea[i].size;
			fdr->dirEntryPosition = i;
			fdr->directoryStartLocation = dea[i].loc;
			fdr->location[count]= i;
			fdr->fileType = dea[i].ford;
			fdr->increment = count;
			count++;
        	}
    	}

    return fdr;
}

/* We are currently listing all the directory entry names (files and directories) that are in the current working
directory. */
struct fs_diriteminfo *fs_readdir(fdDir *dirp){
	dinfo = malloc(sizeof(struct fs_diriteminfo));
	dinfo -> d_reclen = dirp -> d_reclen;
	dinfo -> fileType = dirp -> fileType;
	strcpy(dinfo -> d_name, dea[dirp->location[count2]].dename);
   
	if (count < count2 && strcmp(currentDir,".")==0){
		dinfo = NULL;
		count =0;
		count2 = 0;
    	} else if (count <= count2 && strcmp(currentDir,".")!=0) {
		dinfo = NULL;
		count =0;
		count2 = 0;
    	} else{
		count2++;
   	}

    return dinfo;

}

int fs_closedir(fdDir *dirp){
	free(dirp);
	dirp = NULL;
	return 0;
}

// Get the current working directory.
char * fs_getcwd(char *buf, size_t size){
	return currentDir;
}

/* fs_setcwd is used for cd and when we use cd, we change the currentDir variable to the current working directory
that we just changed to. First, we have to check if the directory exists or not and it can not be like a text file.
We can only change to directories. For example, the current working directory is ./newDir and inside ./newDir, we 
have newDir1. We want to cd newDir1. I simply check if the directory exists in ./newDir and then if it exists, we 
change currentDir variable to ./newDir/newDir1. That's all I need to do. When we do cd .., we want to go back to the previous current working directory which is ./newDir. In order to do that, I just need to change currentDir to 
./newDir by removing /newDir1. I used strtok for this and removed anything after the last dash. */
int fs_setcwd(char *buf){   
	int retVal = -1;
	char path[100];
	char *parameters[100];
	int count = 0;
	if (strcmp(buf, "..")==0 && strcmp(currentDir, ".")!=0){
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
		if (strcmp(buf, dea[i].dename)==0 && dea[i].ford == 0){
			return -1;
		}
		if (strcmp(currentDir,dea[i].currentDir)==0 && strcmp(buf, dea[i].dename)==0){
			retVal = 0;
		}
	}

	if (retVal == 0){
      		char * str3 = (char *) malloc(2 + strlen(currentDir)+ strlen(buf) );
  		strcpy(str3, currentDir);
  		strcat(str3, "/");
      		strcat(str3, buf);
		strcpy(currentDir, str3);
		free(str3);
		str3 = NULL;
		return 0;
	}

	return retVal;
}

int fs_isFile(char * path){	//return 1 if file, 0 otherwise
	for(int i = 0; i < vcbp->sroots; i++){
		if (strcmp(dea[i].dename, path) == 0){
			if (dea[i].ford == 0){
			   return 1;
			}
		}
	}

	return 0;
}

int fs_isDir(char * path){		//return 1 if directory, 0 otherwise
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
		   dea[i].size = -1;
		   dea[i].loc = 0;
		   strcpy(dea[i].dename,"        ");
		   LBAwrite(dea, vcbp->srootbs, vcbp->sroot);
		   return 0;
		}
	}

	return -1;
}

/*ensures that the path passed in exists in the list of directories and files and 
saves grabs the values from the proper file and saves it to the struct fs_stat buf. */
int fs_stat(const char *path, struct fs_stat *buf){
	for (int i = 0; i < vcbp->sroots; i++) {
		if (strcmp(dea[i].dename, path) == 0) {
			buf->st_size = dea[i].size*vcbp->sblk;
			buf->st_blksize = vcbp->sblk;
			buf->st_blocks = dea[i].size;
			//creates a time_t to get a time
			time_t fileTime = time(NULL);
			buf->st_accesstime = fileTime;
			buf->st_createtime = fileTime;
			buf->st_modtime = fileTime;
		} 
	}
	return 0;
}
