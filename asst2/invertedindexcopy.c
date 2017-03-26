#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

char path[4096];

int isFile(const char* path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void tokenize(int fd, char* fname)
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
				char* temp = (char*)realloc(token,i+2);
				token = temp;
				token[i] = buffer[0];
			}
			
			i++;
			if(read(fd,buffer,1) == 0) break;
		}

		if(token != NULL)
		{
			token[i] = '\0';
			printf("%s\n", token);
			token = NULL;
			i = 0;
		}
	}
	return;
}

void indexDir(char* name)
{
	struct dirent * currObj = NULL;
	DIR* directory = opendir(name);
	currObj = readdir(directory);
	//chdir(name);

	while(currObj != NULL )
	{
		if (!strcmp(".", currObj->d_name) || !strcmp("..", currObj->d_name))
		{
			currObj = readdir(directory);
			continue;
		}
		if(currObj->d_type == DT_REG)
		{
			//currObj is a file
			printf("\tfilename: %s\n",currObj->d_name);
			char* temp = (char*)malloc(strlen(path) + strlen(currObj->d_name) + 2);
			strcpy(temp,path);
			strcat(temp,"/");
			strcat(temp,currObj->d_name);
			printf("%s\n",temp);		
			int fd = open(temp,O_RDONLY);
			tokenize(fd, currObj->d_name);
			free(temp);
			close(fd);
		}
		else if(currObj->d_type == DT_DIR)
		{	
			printf("\tdirname: %s\n",currObj->d_name);
			strcat(path,"/");
			strcat(path,currObj->d_name);
			indexDir(path);
		}
		else
		{
			//other
		}
		currObj = readdir(directory);
	}
	return;	
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
		tokenize(fd, argv[2]);
		close(fd);
	}

	//if 2nd argument is a directory(will handle recursive directory traversal after program is working with a single file)
	else
	{
		strcpy(path,"./");
		strcat(path,argv[2]);
		indexDir(path);
	}

	return 0;
}