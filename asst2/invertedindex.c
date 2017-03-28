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
	printf("<?xml version=\"1.0\" encoding=\"UTF-8\">\n<fileIndex>\n");
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\">\n<fileIndex>\n");
	while(ptr != NULL)
	{
		printf("\t<word text=\"%s\">\n", ptr->token);
		fprintf(fp,"\t<word text=\"%s\">\n", ptr->token);
		fileNode* ptr2 = ptr->fHead;
		while(ptr2 != NULL)
		{
			printf("\t\t<file name=\"%s\">%d</file>\n",ptr2->fileName,ptr2->count);
			fprintf(fp,"\t\t<file name=\"%s\">%d</file>\n",ptr2->fileName,ptr2->count);
			ptr2 = ptr2->next;
		}
		printf("\t</word>\n");
		fprintf(fp,"\t</word>\n");
		ptr = ptr->next;
	}
	printf("</fileIndex>\n");
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
		if(wHead != NULL ) if(wHead -> fHead -> next != NULL) printf("%s\n",wHead->fHead->next->fileName);
		currObj = readdir(directory);
	}
	return;	
}

int main(int argc, char** argv)
{
	char path[4096];
	if(argc != 3)
	{
		fprintf(stderr, "ERROR: Invalid number of arguments\nUSAGE: ./invertedindex <inverted-index file name> <directory or file name>\n");
		return 1;
	}

	if(!(isDir(argv[2]) || isFile(argv[2])))
	{
		fprintf(stderr, "ERROR: '%s' is not a file or directory\n",argv[2]);
		return 1;
	}
	FILE* output = fopen(argv[1],"w");

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

	iterate(output);
	fclose(output);

	return 0;
}