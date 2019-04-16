#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *str = malloc(128);
    scanf("%s", str);
    printf("%s\n", str);
    return 0;
}
