#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main() {
    while (1) {
        char *directory = "hw2/";
        char *index = "/httpserver.c";
        char *file_path = malloc(strlen(directory) + strlen(index));
        strncpy(file_path, directory, strlen(directory)-1);
        file_path[strlen(directory)-1] = '\0';
        printf("%s %d\n", file_path, strlen(file_path));

        strcat(file_path, index);
        printf("%s %d\n", file_path, strlen(file_path));
        printf("%s %d\n", "hw2/httpserver.c", strlen("hw2/httpserver.c"));

        FILE *fp = fopen(file_path, "rb+");
        if (fp) {
            printf("is a file\n");
            fclose(fp);
        }
        else if (errno == EISDIR) {
            printf("is a direcotry\n");
        }
        else {
            printf("%d\n", errno);
        }
        free(file_path);
    }
    // FILE *fp = fopen("hw2/httpserver.c", "rb+");
    // fclose(fp);
    return 0;
}
