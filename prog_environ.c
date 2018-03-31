
/* Write a C program that outputs the contents of its Environment list */

#include<stdio.h>

#include<stdlib.h>

int main(void)

{

int i;

char **ptr;

extern char **environ;

printf("ENV VAR are\n");

for(ptr=environ;*ptr!=0;ptr++)

printf("%s\n",*ptr);

exit(0);

}

