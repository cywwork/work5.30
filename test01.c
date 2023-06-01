#include<stdio.h>

int main(){

FILE *fp;

fp = fopen("test01.txt","w");



fclose(fp);

return 0;
}
