#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

// Constant Declarations
#define maxRoomNameLen 80
#define maxRoomConnections 6 

// Struct Declarations
struct Room 
{
   char roomName[maxRoomNameLen];
   char connections[maxRoomConnections][maxRoomNameLen];
   char roomType[maxRoomNameLen];
};
struct stat st = {0};

// Function Declarations
void GenerateRoomsDirectory();
void GenerateAllRoomFiles();
int GenerateRandomNumber(const int minNumber, const int maxNumber, const int timeOffSet);
int AnyElementInArrayEmpty(char **arrayToCheck, int arraySize);
int ElementNotInArray(char **arrayToCheck, int arraySize, const char *element);
int GetEmptySlot(char **arrayToCheck, int arraySize);
void GetRandomElement(char **array, int arraySize, int randomIncrement, char *returnValue);
void RemoveElementByValue(char **array, int arraySize, char *value);
void GetRoomsDirName(char *returnValue, int maxLen);
void GenerateRoomConnections(char **rooms, int numRooms);
void InitializeRoomsArray(struct Room rooms[], int maxRoomNumber, char **roomNames);

// Program entry point
int main()
{
   GenerateRoomsDirectory();
   GenerateAllRoomFiles();
}

void GenerateRoomsDirectory()
{
   char dirName[80];
   GetRoomsDirName(dirName, 80);
  
   // Create rooms directory
   if (stat(dirName, &st) == -1)
   {
      mkdir(dirName, 0700);
   }
}

 /**************************************************************
 * * Entry:
 * *	returnValue  - pointer to hold the return string value
 * *	manLen  - max length of the return value
 * *
 * * Exit:
 * *	No return value
 * *
 * * Purpose:
 * *	To get the room's directory name
 * *
 * ***************************************************************/
void GetRoomsDirName(char *returnValue, int maxLen)
{
   // Get Process Id
   int curPid = getpid();
   char pidString[10];
   snprintf(pidString, sizeof(pidString), "%d", curPid);

   // Generate rooms directory name
   char directoryName[] = "toke.rooms.";
   strcat(directoryName, pidString);

   strncpy(returnValue, directoryName, maxLen);
   //printf(" here\n");
}

// Creates empty room files only
void GenerateAllRoomFiles()
{
   char directoryName[80];
   GetRoomsDirName(directoryName, 80);

   // Create room files
   FILE *filePointer;
   
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

   // Explanation of how the room algorithm works:
   //  For each element in the rooms array we are going to 
   //  find a random element in the possible rooms array. Once 
   //  the random string is found we remove that string from the 
   //  possible rooms list and add it to the rooms array. Each new 
   //  possible room query will only return a random element from
   //  what is left in the possible room array.
   int i = 0;
   char roomName[80];
   char fileName[80];
   for (i = 0; i < maxRoomNumber; i++)
   {
      GetRandomElement(possibleRoomNames, maxRoomNumber, i, roomName);
      RemoveElementByValue(possibleRoomNames, numPossibleRoomNames, roomName);
      roomNames[i] = roomName;

      // Generate the file name
      strncpy(fileName, directoryName, 80);
      strcat(fileName, "/");
      strncat(fileName, roomName, 80);
     
      // Create the files
      filePointer = fopen(fileName, "w");
      fclose(filePointer);
   }

   // TODO
   // 1. Create an adjacency matrix that is 7 by 6. 7 rooms
   //    by max number of connections
   // 2. Determine how many connections in the adjacency array
   //    by either putting "OPEN" or "CLOSED" 
   // 3. Algorithm to connect the rooms:
   //     a. Randomly pick a room.
   //     b. Determine if room has open connections.
   //     c. If room has an open connection, pick another room
   //        with an open connection.
   //     d. Connect the two rooms.
   //     e. Randomly pick another empty room
   //     f. Randomly pick a connected room that has open connections
   //     g. Connect the rooms.
   //

   // Create an array of structs
   struct Room rooms[7];
   InitializeRoomsArray(rooms, maxRoomNumber, roomNames);
   GenerateRoomConnections(roomNames, maxRoomNumber);
}

void InitializeRoomsArray(struct Room rooms[], int maxRoomNumber, char **roomNames)
{
   //struct Room 
   //{
   //   char roomName[maxRoomNameLen];
   //   char connections[maxRoomConnections][maxRoomNameLen];
   //   char roomType[maxRoomNameLen];
   //};

   strncpy(rooms[0].roomName, roomNames[0], 80);
  // printf("%s\n", rooms[0].roomName);
}

void GenerateRoomConnections(char **rooms, int numRooms)
{
   // Connect the two initial rooms first
   char roomName[80]; 
   GetRandomElement(rooms, numRooms, 0, roomName);
   //printf("%s\n", roomName);

   int i;
   for (i = 0; i < numRooms; i++)
   {
     // printf("%s\n", rooms[i]);
   }
}

 /**************************************************************
 * * Entry:
 * *	array  - an array of strings 
 * *	arraySize  - the size of the array
 * *	value - the string value you want to replace with "EMPTY"
 * *
 * * Exit:
 * *	No return value
 * *
 * * Purpose:
 * *	To find the specified string in the array and replace it 
 * *    with "EMPTY"
 * *
 * ***************************************************************/    
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

/**************************************************************
 * * Entry:
 * *	array  - an array of strings. This function will ignore all
 * *             elements with the value of "EMPTY"
 * *	arraySize  - the size of the array
 * *	randomIncrement - offset to make the function generate a 
 * *                      different random number when this function
 * *                      is being used in a loop.
 * *
 * * Exit:
 * *	Returns a random string from a string array.
 * *
 * * Purpose:
 * *	To get a random string from a string array.
 * *
 * ***************************************************************/
void GetRandomElement(char **array, int arraySize, int randomIncrement, char *returnValue)
{
   int randomNumber = GenerateRandomNumber(0, arraySize, randomIncrement);
   int i = 0;

   while (strcmp(array[randomNumber], "EMPTY") == 0)
   {
      i++;
      randomNumber = (randomNumber + i) % 10;
   }

   strncpy(returnValue, array[randomNumber], 80);
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
 * *	timeOffSet - offset to make the function generate a 
 * *                 different random number when this function
 * *                 is being used in a loop.
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
