#include <stdio.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main (int argc, char *argv[]) {
	if (argc != 3) { 
		fputs("Invalid parameters\nFirst parameter - name of the first file\nSecond parameter - name of the second file\n",stderr);
		return -1;
	} else {
		FILE *f1, *f2;
		if ((f1 = fopen(argv[1],"r")) == NULL) 
		{
			fputs("Cannot open the first file\n",stderr);
			return -1;
		} else if ((f2 = fopen(argv[2],"w")) == NULL) {
			fputs("Cannot open the second file\n",stderr);
			return -1;
		} else {
			struct stat buf1;
			if (stat (argv[1], &buf1) == -1) {
				fputs("Unable to fill stat structure\n",stderr);
				return -1;
			} else {
				while (!feof(f1)) {
					char c = getc(f1);
					if (c != EOF) putc(c,f2);
				}
				chmod(argv[2], buf1.st_mode);
				if (fclose(f1)) {
					fputs("Cannot close the first file\n",stderr);
					return -1;
				} else if (fclose(f2)) {
					fputs("Cannot close the second file\n",stderr);
					return -1;
				} else return 0;
			}
		}
	}	
}
