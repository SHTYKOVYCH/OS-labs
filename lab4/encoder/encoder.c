#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    size_t len = 0;
    char string[4096] = {0};
    char key;

    scanf("%c\n", &key);

    char tmp;

    tmp = getc(stdin);
    while (tmp != '\n' && tmp != EOF && tmp != 0) {
        string[len] = tmp;
        len += 1;
        tmp = getc(stdin);
    }

    printf("%c\n", key);

    for (int i = 0; i < len; ++i) {
        printf("%c", string[i] ^ key);
    }

    exit(EXIT_SUCCESS);
}