#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

struct stat st = {0};

// Function Declarations
void GenerateRoomsDirectory();
void GenerateAllRoomFiles();
int GenerateRandomNumber(const int minNumber, const int maxNumber, const int timeOffSet);
int AnyElementInArrayEmpty(char **arrayToCheck, int arraySize);
int ElementNotInArray(char **arrayToCheck, int arraySize, const char *element);
int GetEmptySlot(char **arrayToCheck, int arraySize);
char* GetRandomElement(char **array, int arraySize, int randomIncrement);
void RemoveElementByValue(char **array, int arraySize, char *value);

// Program entry point
int main()
{
   GenerateRoomsDirectory();

   GenerateAllRoomFiles();
}

void GenerateRoomsDirectory()
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

void GenerateAllRoomFiles()
{
   // Create room files
   
   // Array of possible room names 
   int numPossibleRoomNames = 10;
   char *possibleRoomNames[10] = 
   {
      "alpha", "cinna",
      "earth", "haven",
      "helicon","hesperos",
      "trantor", "santanni",
      "sirius", "vega"
   };

   // Array of actual game-time room names
   const int maxRoomNumber = 7;
   char *roomNames[7] = 
   { 
      "EmptyElementHere", "EmptyElementHere", 
      "EmptyElementHere", "EmptyElementHere", 
      "EmptyElementHere", "EmptyElementHere", 
      "EmptyElementHere" 
   }; 

   // This assignment function selects a random spot to start, so 
   // its not truly random
   int i = 0;
   char *roomName;
   for (i = 0; i < maxRoomNumber; i++)
   {
      roomName = GetRandomElement(possibleRoomNames, maxRoomNumber, i);
      RemoveElementByValue(possibleRoomNames, numPossibleRoomNames, roomName);
      roomNames[i] = roomName;
      //roomNames[i] = possibleRoomNames[(randomNumber + i) % 10];
   }

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
     
// Find the specified value and delete it from the array. Replace the 
// value at that position with "EMPTY"
void RemoveElementByValue(char **array, int arraySize, char *value)
{
   int i = 0;
   for (i = 0; i < arraySize; i++)
   {
      if (strcmp(array[i], value) == 0)
      {
	 array[i] = "EMPTY";
      }
   }
}

// Get a random element from the specified array. If we encounter an empty
// element then we try to find the next element that is not empty
char* GetRandomElement(char **array, int arraySize, int randomIncrement)
{
   int randomNumber = GenerateRandomNumber(0, arraySize, randomIncrement);
   int i = 0;

   while (strcmp(array[randomNumber], "EMPTY") == 0)
   {
      i++;
      randomNumber = (randomNumber + i) % 10;
   }

   return array[randomNumber];
}

// Return: All positive numbers mean empty slot found.
//         -1 means no empty slots found
int GetEmptySlot(char **arrayToCheck, int arraySize)
{
   int i = 0;
   for (i = 0; i < arraySize; i++)
   {
      if (arrayToCheck[i] == "EmptyElementHere")
      {
	 return i;
      }
   }
   return -1;
}

// Return: 1 means true and 0 means false
int ElementNotInArray(char **arrayToCheck, int arraySize, const char *element)
{
   int i = 0;
   for (i = 0; i < arraySize; i++)
   {
      if (strcmp(arrayToCheck[i], element) == 0)
      {
	 return 0;
      }
   }
   return 1;
}

// Return: 1 means true and 0 means false
int AnyElementInArrayEmpty(char **arrayToCheck, int arraySize)
{
   int i = 0;
   for (i = 0; i < arraySize; i++)
   {
      if (arrayToCheck[i] == "EmptyElementHere")
      {
	 return 1;
      }
   }
   return 0;
}

/**************************************************************
 * * Entry:
 * *	minNumber - the minimum random number
 * *	maxNumber - the maximum random number
 * *
 * * Exit:
 * *	Returns a randomly generated number.
 * *
 * * Purpose:
 * *	To generate a random number between the min and max.
 * *
 * ***************************************************************/
int GenerateRandomNumber(const int minNumber, const int maxNumber, const int timeOffSet)
{
	
   // Set seed of random number to make number more random.
   srand(time(0) + timeOffSet);

   // Generate random number
   int range = maxNumber - minNumber;
   return (minNumber + (rand() % (range + 1)));
}
