#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int main() {
	char currCatalog[PATH_MAX];
	if ( getcwd(currCatalog, PATH_MAX) == NULL ) {
		fputs("Cannot get name of current catalog\n",stderr);
		return -1;
	}
	else {
		puts("\n");
		printf("Current directory is %s\n", currCatalog);
		puts("-----------------------------------------");
		struct dirent *dirStruct;
		DIR *dir;
		if ((dir = opendir (currCatalog)) == NULL) {
			fputs("Cannot open stream of current catalog\n",stderr);
			return -1;
		}
		else {	
			while (dirStruct = readdir(dir)) {
				printf("%s\n",dirStruct->d_name);
			}
			if (closedir(dir) == -1) {
				fputs("Cannot close stream of current catalog\n",stderr);
				return -1;	
			} 
			puts("\n");
			char rootCatalog[] = "/";	
			if ((dir = opendir (rootCatalog)) == NULL) {
				fputs("Cannot open stream of root catalog\n",stderr);
				return -1;
			} else {
				puts("Root directory:");
				puts("-----------------------------------------");
				while (dirStruct = readdir(dir)) {
					printf("%s\n",dirStruct->d_name);
				}
				if (closedir(dir) == -1) {
					fputs("Cannot close stream of root catalog\n",stderr);
					return -1;	
				}  
				else return 0;
			}
		}
	}
}
