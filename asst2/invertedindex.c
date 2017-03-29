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

typedef struct fileNode
{
	char* fileName;
	int count;
	struct fileNode* next;
} fileNode;

typedef struct wordNode
{
	char* token;
	fileNode* fHead;
	struct wordNode* next;
} wordNode;

static wordNode* wHead = NULL;

void sortFileNodes()
{
 	if(wHead == NULL) return;
 	wordNode* ptr = wHead;
 	while(ptr != NULL)
 	{
 		fileNode* ptr2 = ptr->fHead;
 		fileNode* ptr3 = ptr->fHead;
 		fileNode* max = ptr->fHead;
 		while(ptr2 != NULL)
 		{	
 			ptr3 = ptr2;
 			max = ptr2;
 			while(ptr3 != NULL)
 			{
 				if(ptr3->count > max->count) max = ptr3;
 				else if(ptr3->count == max->count && strcmp(ptr3->fileName,max->fileName) < 0) max = ptr3;
 				ptr3 = ptr3->next;
 			}
 			char* tempFileName = ptr2->fileName;
 			int tempCount = ptr2->count;
 			ptr2->fileName = max->fileName;
 			ptr2->count = max->count;
 			max->fileName = tempFileName;
 			max->count = tempCount;
 			ptr2 = ptr2->next;
 		}
  
 		ptr = ptr->next;
 	}
}
//strcmp in which numbers come after letters
int myStrcmp(char* s1, char* s2)
{
	unsigned char *p1 = (unsigned char *)s1;
    unsigned char *p2 = (unsigned char *)s2;
    while (*p1 != '\0') {
    	if(isdigit(*p1)&&!isdigit(*p2)) return 1;
    	if(!isdigit(*p1)&&isdigit(*p2)) return -1;
        if (*p2 == '\0') return  1;
        if (*p2 > *p1)   return -1;
        if (*p1 > *p2)   return  1;

        p1++;
        p2++;
    }

    if (*p2 != '\0') return -1;
    return 0;
}

void iterate(FILE* fp)
{
	wordNode* ptr = wHead;
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\">\n<fileIndex>\n");
	while(ptr != NULL)
	{
		fprintf(fp,"\t<word text=\"%s\">\n", ptr->token);
		fileNode* ptr2 = ptr->fHead;
		while(ptr2 != NULL)
		{
			fprintf(fp,"\t\t<file name=\"%s\">%d</file>\n",ptr2->fileName,ptr2->count);
			ptr2 = ptr2->next;
		}
		fprintf(fp,"\t</word>\n");
		ptr = ptr->next;
	}
	fprintf(fp,"</fileIndex>\n");
}

void toLowerCase(char* token)
{
	int i;
	for(i=0;i<strlen(token);i++)
	{
		token[i] = tolower(token[i]);
	}
}

fileNode* buildFileNode(char* fileName)
{
	fileNode* newNode = (fileNode*)malloc(sizeof(fileNode));
	newNode->fileName = fileName;
	newNode->count = 1;
	newNode->next = NULL;
	return newNode;
}

wordNode* buildWordNode(char* token, char* fileName)
{
	wordNode* newNode = (wordNode*)malloc(sizeof(wordNode));
	newNode->token = token;
	newNode->fHead = buildFileNode(fileName);
	newNode->next = NULL;
	return newNode;
}

void insertWordNode(char* token, char* fileName)
{
	wordNode* prev = wHead;
	wordNode* ptr = wHead;
	while(ptr != NULL)
	{

		if(myStrcmp(ptr->token,token) > 0)
		{
			if(ptr == wHead)
			{
				wordNode* temp = wHead;
				wHead = buildWordNode(token, fileName);
				wHead->next = temp;
			}
			else
			{
				prev->next = buildWordNode(token,fileName);
				prev->next->next = ptr;
			}
			return;
		}
		prev = ptr;
		ptr = ptr->next;
	}

	prev->next = buildWordNode(token,fileName);
}

fileNode* insertFileNode(fileNode* head, char* fileName)
{
	fileNode* prev = head;
	fileNode* ptr = head;
	while(ptr != NULL)
	{
		if(myStrcmp(ptr->fileName,fileName) > 0)
		{
			if(ptr == head)
			{
				fileNode* temp = head;
				head = buildFileNode(fileName);
				head->next = temp;
			}
			else
			{
				prev->next = buildFileNode(fileName);
				prev->next->next = ptr;
			}
			return head;
		}
		prev = ptr;
		ptr = ptr->next;
	}

	prev->next = buildFileNode(fileName);	
	return head;
}

void addEntry(char* token, char* fileName)
{
	if(wHead == NULL)
	{
		wHead = buildWordNode(token, fileName);
	}
	else
	{
		wordNode* ptr = wHead;
		while(ptr != NULL)
		{
			if(strcmp(ptr->token,token)==0) break;	
			ptr = ptr->next;
		}
		
		if(ptr != NULL)
		{
			fileNode* ptr2 = ptr->fHead;	
			while(ptr2 != NULL)
			{
				//printf("%s\n",ptr2->next->fileName);
				if(strcmp(ptr2->fileName,fileName)==0) break;	
				ptr2 = ptr2->next;				
			}

			if(ptr2 != NULL) ptr2->count++;
			else ptr->fHead = insertFileNode(ptr->fHead,fileName);
		}
		else
		{
			insertWordNode(token, fileName);
		}
	}
	return;
}

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

void tokenize(int fd, char* fname)
{
	char* token = NULL;
	int i = 0;
	char buffer[1];
	while(read(fd,buffer,1) != 0)
	{
		while(isalnum(buffer[0]) && !(token == NULL && isdigit(buffer[0])))
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
			toLowerCase(token);
			addEntry(token,fname);
			token = NULL;
			i = 0;
		}
	}
	return;
}

void indexDir(char name[4096])
{
	struct dirent * currObj = NULL;
	DIR* directory = opendir(name);
	currObj = readdir(directory);
	while(currObj != NULL )
	{
		char* fname = (char*)malloc(strlen(currObj->d_name)+1);
		strcpy(fname,currObj->d_name);

		if (!strcmp(".", currObj->d_name) || !strcmp("..", currObj->d_name))
		{
			currObj = readdir(directory);
			continue;
		}
		if(currObj->d_type == DT_REG)
		{
			//currObj is a file
			char* temp = (char*)malloc(strlen(name) + strlen(currObj->d_name) + 2);
			strcpy(temp,name);
			strcat(temp,"/");
			strcat(temp,currObj->d_name);
			int fd = open(temp,O_RDONLY);
			tokenize(fd, fname);
			free(temp);
			close(fd);
		}
		else if(currObj->d_type == DT_DIR)
		{	
			char* temp = (char*)malloc(strlen(name) + strlen(currObj->d_name) + 2);
			strcpy(temp,name);
			strcat(temp,"/");
			strcat(temp,currObj->d_name);
			indexDir(temp);
			free(temp);
		}
		currObj = readdir(directory);
	}
	closedir(directory);
	return;	
}

int main(int argc, char** argv)
{
	//char path[4096];
	if(argc != 3)
	{
		fprintf(stderr, "ERROR: Invalid number of arguments\nUSAGE: ./invertedindex <inverted-index file name> <directory or file name>\n");
		return 1;
	}

	char* tempFileName = (char*)malloc(strlen(argv[2]));

	if(argv[2][strlen(argv[2])-1] == '/')
	{
		strncpy(tempFileName, argv[2], strlen(argv[2])-1);
		tempFileName[strlen(argv[2])] = '\0';
	}
	else
	{
		strcpy(tempFileName,argv[2]);
	}

	if(!(isDir(tempFileName) || isFile(tempFileName)))
	{
		fprintf(stderr, "ERROR: '%s' is not a file or directory\n",argv[2]);
		return 1;
	}

	if(access(argv[1], F_OK ) != -1) 
	{
		printf("WARNING: File '%s' already exists, would you like to overwrite? (y/n)\n", argv[1]);
		char input;
		scanf("%c",&input);
		while(input != 'y' && input != 'n')
		{
			printf("Invalid input, please enter either 'y' or 'n'.\n");
			scanf("%c\n",&input);
		}
		if(input == 'n')
		{
			printf("Exiting...\n");
			return 0;
		}
	} 
  
	FILE* output = fopen(argv[1],"w");

	//if 2nd argument is a file
	if(isFile(tempFileName))
	{
		int fd = open(tempFileName,O_RDONLY);
		tokenize(fd, tempFileName);
		close(fd);
	}

	//if 2nd argument is a directory(will handle recursive directory traversal after program is working with a single file)
	else
	{
		indexDir(tempFileName);
	}

	sortFileNodes();
	iterate(output);
	fclose(output);
	free(tempFileName);

	printf("Output in file: '%s'...\n",argv[1]);

	return 0;
}