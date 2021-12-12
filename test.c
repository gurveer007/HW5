#include<string.h>
#include<stdio.h>
int main()
{
    char buf[200];
    //encoding the grid into buf (100 chars)
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
                if (x==0 && y==0)
                {
                    strcpy(buf,"0,");
                }else{
                    strcat(buf, "0,");
                }
                
                
            
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

    //replacing last "," with termination character
    // if (buf != -1) {
        // buf[strlen(buf)-1] = '\0';
    // }
    
    // char buf[100] = "hello is";
    char *p;
    p = strtok(buf, " ");

    
    for (size_t i = 0; i < strlen(buf); i++)
    {
        if(p)
        {
            printf("%s\n", p);
        }
        p = strtok(NULL, ",");
    }
    
    return 0;
}