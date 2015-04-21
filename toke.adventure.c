#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat st = {0};

// Function Declarations
void generateRoomsDirectory();
void generateAllRoomFiles();

// Program entry point
int main()
{
   generateRoomsDirectory();
   generateAllRoomFiles();
}

void generateRoomsDirectory()
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

void generateAllRoomFiles()
{
   // Create room files
   
   // Create a list of possible room names
   const int numPossibleRoomNames = 10;
   const char *possibleRoomNames[10] = 
   {
      "alpha", "cinna",
      "earth", "haven",
      "helicon","hesperos",
      "trantor", "santanni",
      "sirius", "vega"
   };

   // Create an array of size 7
   int i = 0;
   const int maxRoomNumber = 7;
   char *roomNames[7] = { NULL }; 

   // Generate a random number between 0 and 9 inclusive
   // Check if posssible[random] string is in roomNames

   // Use this to test the room names
   for (i = 0; i < maxRoomNumber; i++)
   {
      printf("name %d: %s\n", i, roomNames[i]);
   }

   // 	  For each slot in array generate a random number
   // 	   populate the size 7 array with the random index from
   // 	   array size 10. check if the size 7 array already has the 
   // 	   name if it does then increment the random number and check 
   // 	   again
   // 	
   // Create room connections
}
