#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sdshdr {
    int len;
    int free;
    char buf[];
};



int main() {
    struct sdshdr *sh;
    sh = malloc(sizeof(struct sdshdr)+5+1);
    sh->len = 5;
    sh->free = 0;
    memcpy(sh->buf,"hello",5);
    sh->buf[5] = '\0';
    printf("sizeof int: %ld\n", sizeof(int));
    printf("sdshdr address: %p\n", sh);
    printf("buf address: %p\n", sh->buf);
    printf("-- :%p\n", sh->buf-(sizeof(struct sdshdr)));
    return 0;
}
