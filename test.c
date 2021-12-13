#include<string.h>
#include<stdio.h>
int main()
{
    char buf[300]= "";
    //encoding the grid into buf (100 chars)
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
                // if (x==0 && y==0)
                // {
                //     strcat(buf,"0,");
                // }else{
                //     strcat(buf, "0,");
                // }
                strcat(buf, "0,");
        }
    }
    char temp[10];
    int score = 10;
    int numTomatoes = 9;
    int level = 7;
    int localPlayerId = 1;

    sprintf(temp, "%d", score);
    strcat(buf, temp);
    strcat(buf, ",");

    sprintf(temp, "%d", numTomatoes);
    strcat(buf, temp);
    strcat(buf, ",");

    sprintf(temp, "%d", level);
    strcat(buf, temp);
    strcat(buf, ",");
    
    sprintf(temp, "%d", localPlayerId);
    strcat(buf, temp);
    strcat(buf, ",");
    strcat(buf, "\n");

    //replacing last "," with termination character
    if (buf) {
        buf[strlen(buf)-1] = '\0';
    }
    
    // char buf[100] = "hello is";
    //printf("1st length of buf is: %ld\n", strlen(buf));
    int length = strlen(buf);
    char * temp1[300];
    int tempcounter = 0;
    char *p;
    p = strtok(buf, ",");
    int count =0;
    //printf("2nd length of buf is: %ld\n", strlen(buf));
    for (size_t i = 0; i < length; i++)
    {
        if(p)
        {
          //if (strcmp(p,"0") == 0) {
            //printf("%s\n", p);
            //count++;
          //}
          temp1[i] = p;
          if (strcmp(temp1[i],"0") == 0) {
              printf("it works\n");
          }
          printf("the number is: %s\n", temp1[i]);
        }
        p = strtok(NULL, ",");
    }
    printf("the count is %d\n", count);
    return 0;
}