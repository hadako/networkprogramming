#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void open_img(void) {
    FILE    *fp;
    char    buf[1024];
    int    i, size;
    fp = fopen("img.jpg", "rb");
    if(fp != NULL) {
        while((size = fread( buf, sizeof(unsigned char), 1024, fp ))>0) {
            for(i = 0 ; i < size ; i++) {
                printf("%c", buf[i]);
            }
        }
        fclose(fp);
    }
}

int main(void) {
    int    count;
    open_img();
    return 0;
}