#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int isFile(const char* path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
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
	else{
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