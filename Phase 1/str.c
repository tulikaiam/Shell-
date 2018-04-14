#include <stdio.h>
#include <string.h>
#include<stdlib.h>
char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}

int main(void)
{
  char *string;
	string=(char*)malloc(100*sizeof(char));
string=replace_str("Hello, world!", "world", "Miami");
	puts(string);
  return 0;
}
