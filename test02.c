#include<stdio.h>

int main(){

FILE *fp;

fp = fopen("test02.txt","w");

fprintf(fp,"chenyuanwei");

fclose(fp);

return 0;
}

