#include <stdio.h>
#include <string.h>

int main()
{
   int curPid = getpid();
   char pidString[10];
   snprintf(pidString, sizeof(pidString), "%d", curPid);

   char directoryName[] = "toke.rooms.";
   strcat(directoryName, pidString);
   printf("dir name: %s\n", directoryName);
}
