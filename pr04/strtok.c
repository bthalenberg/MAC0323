
/* strtok test */
#include <stdio.h>
#include <string.h>

int main ()
{
  char s[] = "start   ADD   a,a,1";
  char *token;
  char *words[100];
  int i = 0;
  token = strtok(s, " ,");
  while(token != NULL){
    words[i++] = token;
    token = strtok (NULL, " ,");
  } 
  for(int j = 0; j < i; j++){
    printf ("%s\n", words[j]);
  }

  return 0;
}
