#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat st = {0};

int main()
{
   // Get Process Id
   int curPid = getpid();
   char pidString[10];
   snprintf(pidString, sizeof(pidString), "%d", curPid);

   // Generate rooms directory name
   char directoryName[] = "toke.rooms.";
   strcat(directoryName, pidString);

   // Create rooms directory
   if (stat(directoryName, &st) == -1)
   {
      mkdir(directoryName, 0700);
   }
}
