#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define PATH_MAX 4096 


typedef struct fileTokenCount{
	char* fileName;
	int count;
} fileTokenCount;

typedef struct indexMap{
	char* token;
	fileTokenCount files[10];
} indexMap;

int isFile(const char* path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int isDir(const char* path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

void tokenize(int fd)
{
	char* token = NULL;
	int i = 0;
	char buffer[1];
	while(read(fd,buffer,1) != 0)
	{
		while(isalnum(buffer[0]))
		{
			if(token == NULL)
			{
				token = (char*)malloc(2);
				token[0] = buffer[0];
			}
			else
			{
				char* temp = (char*)realloc(token,i+1);
				token = temp;
				token[i] = buffer[0];
			}
			i++;
			read(fd,buffer,1);
		}
		token[i] = '\0';
		printf("%s\n", token);
		token = NULL;
		i = 0;
	}

}

 void recursiveIndex(const char* dirName, DIR* dstream, char * path){
	//Need to find way to maintain absoulte path as we go into child directories , 
	//currently program has issues because it searches for target in current working directory insted of the child directory

	printf("recursive index called\n");
	DIR* directory;
	
	
	if (dstream==NULL){
		printf("dir stream was null\n");
	directory = opendir(dirName);
	if (directory==NULL){
		printf("directory was empty\n");
		return;
	}}
	else{
		directory = dstream;
	}

	char fullpath[PATH_MAX];
	
	if (strcmp("",path)==0){
		getcwd(fullpath,PATH_MAX);
	}
	path=fullpath;
	
	
// get next element in dir
	struct dirent* entry= readdir(directory);
	// while ((strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)){
	// 	entry = readdir(directory);
	// }
	//need to make it so that when recursive index is called on a subdirectory, the path is updated so that it is still relitive to the working directory
	
//ignore refrences to hidden files and placeholders
	while(entry->d_name[0]=='.'){
		entry = readdir(directory);
	}
			// char * newPath;
			// newPath = malloc(strlen(entry->d_name)+strlen(fullpath)+2);
   //  		newPath[0] = '\0';   // ensures the memory is an empty string
   //  		strcat(newPath,fullpath);
   //  		strcat(newPath,entry->d_name);
    		
    		//no elements left in dir
		if (entry == NULL){
			printf("entry was null \n");
			closedir(directory);
			return;
		}
		//if dirent is file, pass file to tokenizer, pass dirstream back to recursive index to process next element in this directory. 
		else if (isFile(newPath)){
			printf("%swas a file\n",entry->d_name);
			int fd = open(entry->d_name,O_RDONLY);
			tokenize(fd);
			recursiveIndex(NULL,directory,fullpath);
			return;
		}
		//if file from stream is a child directory, call recursiveIndex on that directory and pass in dirstream to that directory. 
		else if (isDir(newPath)){
			printf("%swas a dir\n", entry->d_name);
			DIR* childDir = opendir(entry->d_name);
			//strcat(newPath,"/");
    		printf("%snew path is \n", newPath);
			recursiveIndex(NULL,childDir,newPath);
			return;
		}
		else{
			printf("there was a problem with file %s\n", entry->d_name);
		}
}



int main(int argc, char** argv)
{
	if(argc != 3)
	{
		fprintf(stderr, "ERROR: Invalid number of arguments\nUSAGE: ./invertedindex <inverted-index file name> <directory or file name>\n");
		return 1;
	}

	//FILE* output = fopen(argv[1],"w");

	//if 2nd argument is a file
	if(isFile(argv[2]))
	{
		int fd = open(argv[2],O_RDONLY);
		tokenize(fd);
	}

	//if 2nd argument is a directory(will handle recursive directory traversal after program is working with a single file)
	else if (isDir(argv[2])){
		recursiveIndex(argv[2],NULL,"");
	}
	
	else{
		printf("thats a bop\n");
		struct dirent * currObj = NULL;
		DIR* directory = opendir(argv[2]);
		currObj = readdir(directory);
		if(currObj != NULL)
		{
			if(currObj->d_type == DT_REG)
			{
				//currObj is a file
				int fd = open(argv[2],O_RDONLY);
				tokenize(fd);
			}
			else if(currObj->d_type == DT_DIR)
			{
				//currObj is a directory
			}
			else
			{
				//other
			}
		}
	}

	return 0;
}