#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    char* file;
    if (argc == 1) {
        file = malloc(sizeof(char)*128);
	scanf("%s", file);
    }
    else {
        file = argv[1];
    }
//    printf("%s\n", file);
    FILE *fp;
    fp = fopen(file, "r");
    int lines = 0, words = 0, bytes = 0;
    int c = fgetc(fp);
    bool blank = true;
    while (c != EOF) {
//	printf("%c", c);
        bytes++;
	if (c == ' ' || c == '\t') {  // Attention: Don't forget '\t'
	    if (!blank)
	    	words++;
	    blank = true;
	}
	else if (c == '\n') {
	    lines++;
	    if (!blank)
		words++;
	    blank = true;
	}
	else {
	    blank = false;
        }
        c = fgetc(fp);
    }
    printf("%d %d %d %s\n", lines, words, bytes, file);
    return 0;
}
