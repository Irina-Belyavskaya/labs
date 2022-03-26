#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

int numDirectories = 0;
int numFiles = 0;
int isFind = 0;
char *file = NULL;

char *concatenation(char *str1, char *str2) {
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	char *result = calloc(len1 + len2 + 1, sizeof(char));
	if (!result) {
		perror("no memory:");
		puts("\n");
		return NULL;
	}

	strcat(result, str1);
	strcat(result + len1, str2);

	return result;
}

int findFile(char *directory) {
	int result = 0;
	int isBufferAllocated = 0;

	if (directory[strlen(directory) - 1] != '/') {
		directory = concatenation(directory, "/");
		if (!directory)	{
			return -1;
		}
		isBufferAllocated = 1;
	}

	DIR *dir = opendir(directory);
	if (!dir) {
		fprintf(stderr, "%s: %s\n", directory, strerror(errno));
		result = -1;
		goto release_resources;
	}

	for (struct dirent *dirInfo = readdir(dir); dirInfo != NULL; dirInfo = readdir(dir)) {
	
		if (dirInfo->d_type == DT_DIR && // isDirectory
			(strcmp(dirInfo->d_name, ".") != 0 && strcmp(dirInfo->d_name, "..") != 0)) {
			
			char *dirPath = concatenation(directory, dirInfo->d_name);
			if (!dirPath) {
				result = -1;
				goto release_resources;
			}

			findFile(dirPath);
			
			++numDirectories;

			free(dirPath);
		}
		else if (dirInfo->d_type == DT_REG) {
		
			++numFiles;
			
			if (!strcmp(dirInfo->d_name, file)) { // file found
			
				char *FullNameOfFile = realpath(directory,  NULL); 
				printf("\nWay to the file: %s\n", FullNameOfFile);
				free (FullNameOfFile);
				
				char *filePath = concatenation(directory, dirInfo->d_name);
				if (!filePath) {
					result = -1;
					goto release_resources;
				}

				struct stat fileInfo = {0};
				stat(filePath, &fileInfo);

				free(filePath);

				printf("Size:  %ld bytes\n", fileInfo.st_size);

				struct tm *timeInfo;
				timeInfo = localtime(&fileInfo.st_mtime);
				printf("Last time of modification: %s", asctime(timeInfo));

				printf("Acess rights: %o\n", fileInfo.st_mode);

				printf("Inode number: %ld\n\n", fileInfo.st_ino);
			}
		}
	}

release_resources:
	if (isBufferAllocated) {
		free(directory);
	}
	if (dir) {
		closedir(dir);
	}
	return result;
}

int main(int argc, char *argv[])
{
	if (argc != 3)	{
		fputs("Invalid parameters\nFirst parameter - name of catalog\nSecond parameter - name of searching file\n", stderr);
		return -1;
	}

	file = argv[2];
	if (findFile(argv[1]) == 0) {
		printf("\nViewed directories: %d\nViewed files: %d\n\n", numDirectories, numFiles);
	}
	else {
		return -1;
	}
}
