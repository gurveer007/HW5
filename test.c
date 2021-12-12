/* strcat example */
#include <stdio.h>
#include <string.h>

int main ()
{
  char str[80];
  strcpy (str,"this ");
  strcat (str,"number ");
  strcat (str,"is ");
  int num = 100;
//   strcpy (str,num);
//   sprintf();
//   strcat (str,num);
    char temp[10];
    sprintf(temp, "%d", num);
    // printf("%s",temp);
    strcat (str,temp);
  puts (str);
  return 0;
}