#include <stdio.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


int numDirectories = 0;
int numFiles = 0;
int isFind = 0;

char *concatenation (char *str1, char *str2)
{
	size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);                      

	char *result = malloc(len1 + len2 + 1);
	
	memcpy(result, str1, len1);
	memcpy(result + len1, str2, len2 + 1);   
	
	return result; 
}

int findFile(char *directory, char *file)
{
    DIR *dir;
    struct dirent *dirInfo;
    if ( directory[strlen(directory) - 1] != '/' ) directory = concatenation (directory,"/");
  
    if ((dir = opendir(directory)) != NULL)  {
        while ((dirInfo = readdir(dir)) != NULL) {
			char *tmp = NULL;
            if (strcmp(dirInfo->d_name, ".") != 0 && strcmp(dirInfo->d_name, "..") != 0 && strcmp(dirInfo->d_name, "sys") != 0 && strcmp(dirInfo->d_name, "snap") != 0) {
				tmp = concatenation(directory,dirInfo->d_name);	
				if (opendir(tmp) != NULL) {
					numDirectories++;
					//printf("Go to %s\n",directory);
					findFile(tmp, file);
				} else {
					numFiles++;
				}
			}
		
			if (!strcmp(dirInfo->d_name, file))
			{
				puts("\n");
				printf("Way to the file: %s\n", directory);
				
				struct stat fileInfo = { 0 };
				tmp = concatenation(directory,dirInfo->d_name);
				stat(tmp, &fileInfo);
				printf("Size:  %ld bytes\n", fileInfo.st_size);
				
				struct tm *timeInfo;  		 
				timeInfo = localtime (&fileInfo.st_mtime);
				printf("Last time of modification: %s", asctime(timeInfo));
			
				printf("Acess rights: %d\n", fileInfo.st_mode);
			
				printf("Inode number: %ld\n\n", fileInfo.st_ino);
				puts("\n");
			}
			free(tmp);
        }  
    } else {
	//printf("Out of %s\n",directory);
	perror(directory);
     	return -1;
    }
    
    closedir(dir);
    return 0;
}

int main (int argc, char *argv[]) {
	if (argc != 3) {
		fputs("Invalid parameters\nFirst parameter - name of catalog\nSecond parameter - name of searching file\n",stderr);
        return -1;
	}	

	if (findFile(argv[1],argv[2]) == 0) {
		printf("Viewed directories: %d\nViewed files: %d\n", numDirectories, numFiles);  
	} else 
		return -1;
}


