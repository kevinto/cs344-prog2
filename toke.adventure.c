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
void GetRandomElementFrom2D(int maxChar, char array[][maxChar], int arraySize, int randomIncrement, char *returnValue);
void RemoveElementByValue(char **array, int arraySize, char *value);
void RemoveArrayElementByValue(int maxChar, char array[][maxChar], int arraySize, char *value);
void AddElementToArrayByValue(int maxChar, char array[][maxChar], int arraySize, char *value);
void GetRoomsDirName(char *returnValue, int maxLen);
void GenerateRoomConnections(struct Room rooms[], int numRooms, int maxChar, char roomNames[][maxChar]);
void GenerateRoomConnections2(struct Room rooms[], int numRooms, int maxChar, char roomNames[][maxChar]);
void InitializeRoomsArray(struct Room rooms[], int maxRoomNumber, int maxChar, char roomNames[][maxChar]);
void ConnectRooms(struct Room rooms[], int numRooms, char *roomToConnectTo, char *roomForConnection);
void GetRoomWithOpenConn(struct Room rooms[], int numRooms, char *returnRoomName);

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
   char roomNames[7][80];

   // Explanation of how the room algorithm works:
   //  For each element in the rooms array we are going to 
   //  find a random element in the possible rooms array. Once 
   //  the random string is found we remove that string from the 
   //  possible rooms list and add it to the rooms array. Each new 
   //  possible room query will only return a random element from
   //  what is left in the possible room array.
   int i = 0;
   char fileName[80];
   char roomName[80];
   for (i = 0; i < maxRoomNumber; i++)
   {
      GetRandomElement(possibleRoomNames, maxRoomNumber, i, roomName);
      RemoveElementByValue(possibleRoomNames, numPossibleRoomNames, roomName);
      strncpy(roomNames[i], roomName, 80);

      // Generate the file name
      strncpy(fileName, directoryName, 80);
      strcat(fileName, "/");
      strncat(fileName, roomName, 80);
     
      // Create the files
      filePointer = fopen(fileName, "w");
      fclose(filePointer);
   }

   // Create an array of structs
   struct Room rooms[7];
   InitializeRoomsArray(rooms, maxRoomNumber, 80, roomNames);
   GenerateRoomConnections2(rooms, maxRoomNumber, 80, roomNames);
}

void InitializeRoomsArray(struct Room rooms[], int maxRoomNumber, int maxChar, char roomNames[][maxChar])
{
   // Pick a starting room and an end room. Remember the start room
   // cannot be the same as the end room.
   int startingRoomNumber = GenerateRandomNumber(1, 6, 0);
   int endingRoomNumber = startingRoomNumber;
   int k = 0;
   while (endingRoomNumber == startingRoomNumber)
   {
      k++;
      endingRoomNumber = GenerateRandomNumber(1, 6, k);
   }

   // Initiate the array of room structs
   int randomNumber;
   int i;
   for (i = 0; i < maxRoomNumber; i++)
   {
      strncpy(rooms[i].roomName, roomNames[i], 80);

      // Assign the room types
      if (i == startingRoomNumber)
      {
         strncpy(rooms[i].roomType, "START_ROOM", 80);
      }
      else if (i == endingRoomNumber)
      {
         strncpy(rooms[i].roomType, "END_ROOM", 80);
      }
      else 
      {
         strncpy(rooms[i].roomType, "MID_ROOM", 80);
      }

      // Assign a random number of open connections. This means how 
      // many rooms the current room can connect to.
      int j;
      randomNumber = GenerateRandomNumber(3, 6, i);
      for (j = 0; j < randomNumber; j++)
      {
         strncpy(rooms[i].connections[j], "OPEN", 80);
      }

      // Close all of the room connections that arent open 
      // to any other rooms
      int k;
      for (k = 0; k < maxRoomConnections; k++)
      {
         if (strcmp(rooms[i].connections[k], "OPEN") != 0)
         {
          strncpy(rooms[i].connections[k], "CLOSED", 80);
         }
      }
   }
}

void GenerateRoomConnections2(struct Room rooms[], int numRooms, int maxChar, char roomNames[][maxChar])
{
   int i;
   for (i = 0; i < numRooms; i++)
   {
      // Connect the current room with the next room
      strncpy(rooms[i].connections[0], rooms[(i + 1) % numRooms].roomName, 80);

      // Connect the current room to the previous room
      if (i == 0)
      {
         // The first room must connect to the last room
         strncpy(rooms[i].connections[1], rooms[numRooms - 1].roomName, 80);
         continue;
      }
      strncpy(rooms[i].connections[1], rooms[(i - 1) % numRooms].roomName, 80);

      int j;
      for (j = 0; j < maxRoomConnections; j++)
      {
         // printf("room connection %d: %s\n", j, rooms[i].connections[j]);
      }
   }

   // Print out all the objects
   for (i = 0; i < numRooms; i++)
   {
      printf("--------------room name: %s\n", rooms[i].roomName);
      printf("room type: %s\n", rooms[i].roomType);

      int j;
      for (j = 0; j < maxRoomConnections; j++)
      {
         printf("room connection %d: %s\n", j, rooms[i].connections[j]);
      }
   }
}

void GenerateRoomConnections(struct Room rooms[], int numRooms, int maxChar, char roomNames[][maxChar])
{
   int i;
   char connectedRoomNames[7][80];

   for (i = 0; i < 7; i++)
   {
      strncpy(connectedRoomNames[i], "EMPTY", 80);
   }

   // -- Connect the two initial rooms first -- 

   // Get the first room to connect
   char firstRoom[80]; 
   GetRandomElementFrom2D(80, roomNames, numRooms, 0, firstRoom);
   RemoveArrayElementByValue(80, roomNames, 7, firstRoom);
   AddElementToArrayByValue(80, connectedRoomNames, 7, firstRoom);

   // Get the second room to connect
   char secondRoom[80]; 
   GetRandomElementFrom2D(80, roomNames, numRooms, 1, secondRoom);
   RemoveArrayElementByValue(80, roomNames, 7, secondRoom);
   AddElementToArrayByValue(80, connectedRoomNames, 7, secondRoom);

   int startRoomAdded = 0;
   int endRoomAdded = 0;
   int isFirstRoom;
   int isSecondRoom;
   char startRoomName[80];
   char endRoomName[80];
   for (i = 0; i < numRooms; i++)
   {
      // Connect the first room and the second room
      if (strcmp(rooms[i].roomName, firstRoom) == 0)
      {
         strncpy(rooms[i].connections[0], secondRoom, 80);
      }
      else if (strcmp(rooms[i].roomName, secondRoom) == 0)
      {
         strncpy(rooms[i].connections[0], firstRoom, 80);
      }

      // Determine of the start room or end room was added 
      if (strcmp(rooms[i].roomName, firstRoom) == 0 && strcmp(rooms[i].roomType, "START_ROOM") == 0)
      {
         startRoomAdded = 1;
      }
      else if (strcmp(rooms[i].roomName, secondRoom) == 0 && strcmp(rooms[i].roomType, "END_ROOM") == 0)
      {
         endRoomAdded = 1;
      }

      // Get the names of the start room and the end room
      if (strcmp(rooms[i].roomType, "START_ROOM") == 0)
      {
         strncpy(startRoomName, rooms[i].roomName, 80);
      }
      else if (strcmp(rooms[i].roomType, "END_ROOM") == 0)
      {
         strncpy(endRoomName, rooms[i].roomName, 80);
      }
   }

   // Add the start room to the connected rooms, if needed
   char roomToConnectTo[80];   
   if (startRoomAdded != 1)
   {
      GetRandomElementFrom2D(80, connectedRoomNames, 7, 1, roomToConnectTo);
      ConnectRooms(rooms, numRooms, roomToConnectTo, startRoomName);

      RemoveArrayElementByValue(80, roomNames, 7, startRoomName);
      AddElementToArrayByValue(80, connectedRoomNames, 7, startRoomName);
   }

   // Add the end room to the connected rooms, if needed
   if (endRoomAdded != 1)
   {
      GetRandomElementFrom2D(80, connectedRoomNames, 7, 1, roomToConnectTo);
      ConnectRooms(rooms, numRooms, roomToConnectTo, endRoomName);

      RemoveArrayElementByValue(80, roomNames, 7, endRoomName);
      AddElementToArrayByValue(80, connectedRoomNames, 7, endRoomName);
   }

   // Now you have at most 4 rooms connected. Get the rooms that
   // are connected to nothing and connect them to the connected rooms. 
   // Just go through each non-connected room and add them randomly to any open rooms.
   //
   // Now you have all the rooms connected.
   //
   // look for any open connections and connect them
   // This is correct..

   char availConnectedRoom[80];
   for (i = 0; i < numRooms; i++)
   {
      if (strcmp(roomNames[i], "EMPTY") != 0)
      {
         //GetRandomElementFrom2D(80, roomNames, 7, 1, roomToConnectTo);
         //GetRoomWithOpenConn(rooms, numRooms, availConnectedRoom);
         //ConnectRooms(rooms, numRooms, roomToConnectTo, endRoomName);
         AddElementToArrayByValue(80, connectedRoomNames, 7, roomNames[i]);
         RemoveArrayElementByValue(80, roomNames, 7, roomNames[i]);

         // just do 1 for now
         break;
      }
   }

   // This is correct..
   for (i = 0; i < numRooms; i++)
   {
      printf("connected rooms: %s\n", connectedRoomNames[i]);
   }

   for (i = 0; i < numRooms; i++)
   {
      printf("unconnected rooms: %s\n", roomNames[i]);
   }

   // Print out all the objects
   for (i = 0; i < numRooms; i++)
   {
      printf("room name: %s\n", rooms[i].roomName);
      printf("room type: %s\n", rooms[i].roomType);

      int j;
      for (j = 0; j < maxRoomConnections; j++)
      {
         printf("room connection %d: %s\n", j, rooms[i].connections[j]);
      }
   }
}

void GetRoomWithOpenConn(struct Room rooms[], int numRooms, char *returnRoomName)
{
   // the PROBLEM here is that we do not know which rooms are connected..
   int i;
   for (i = 0; i < numRooms; i++)
   {
      printf("room name: %s\n", rooms[i].roomName);

      int j;
      for (j = 0; j < maxRoomConnections; j++)
      {
         printf("room connection %d: %s\n", j, rooms[i].connections[j]);
         if (strcmp(rooms[i].connections[j], "OPEN") == 0)
         {
            strncpy(returnRoomName, rooms[i].roomName, 80);
            return;
         }
      }
   }

   // Just in case, we dont have any open connections
   returnRoomName = "EMPTY";
}

// Connects two rooms to each other
void ConnectRooms(struct Room rooms[], int numRooms, char *roomToConnectTo, char *roomForConnection)
{
   int i;
   int j;
   int firstConnectionMade = 0;
   int secondConnectionMade = 0;
   
   // Connect the "room for connection to the "room to be connected to"
   for (i = 0; i < numRooms; i++)
   {
      // Find the room to connect to
      if (strcmp(rooms[i].roomName, roomToConnectTo) == 0)
      {
         // Look for an open connection
         for (j = 0; j < maxRoomConnections; j++)
         {
            // Do not connect a room to itself
            if (strcmp(rooms[i].connections[j], roomToConnectTo) == 0)
            {
               firstConnectionMade = 1;
               break;
            }

            // If here is already pre-existing connection then get out
            if (strcmp(rooms[i].connections[j], roomForConnection) == 0)
            {
               firstConnectionMade = 1;
               break;
            }

            // Found an open slot, connect the rooms
            if (strcmp(rooms[i].connections[j], "OPEN") == 0)
            {
               strncpy(rooms[i].connections[j], roomForConnection, 80);
               firstConnectionMade = 1;
               break;
            }
         }
      }

      if (firstConnectionMade == 1)
      {
         break;
      }
   }

   // Connect the "room to be connected to" to the "room for connection"
   for (i = 0; i < numRooms; i++)
   {
      // Find the room to connect to
      if (strcmp(rooms[i].roomName, roomForConnection) == 0)
      {
         // Look for an open connection
         for (j = 0; j < maxRoomConnections; j++)
         {
            // Do not connect a room to itself
            if (strcmp(rooms[i].connections[j], roomForConnection) == 0)
            {
               secondConnectionMade = 1;
               break;
            }

            // If here is already pre-existing connection then get out
            if (strcmp(rooms[i].connections[j], roomToConnectTo) == 0)
            {
               secondConnectionMade = 1;
               break;
            }

            // Found an open slot, connect the rooms
            if (strcmp(rooms[i].connections[j], "OPEN") == 0)
            {
               strncpy(rooms[i].connections[j], roomToConnectTo, 80);
               secondConnectionMade = 1;
               break;
            }
         }
      }

      if (secondConnectionMade == 1)
      {
         break;
      }
   }
}

void AddElementToArrayByValue(int maxChar, char array[][maxChar], int arraySize, char *value)
{
   int i;
   for (i = 0; i < arraySize; i++)
   {
      // If element is already in the array then do not add it.
      if (strcmp(array[i], value) == 0)
      {
         return;
      }

      if (strcmp(array[i], "EMPTY") == 0)
      {
         strncpy(array[i], value, maxChar);
         return;
      }
   }
}

// this method takes char array[][]
void RemoveArrayElementByValue(int maxChar, char array[][maxChar], int arraySize, char *value)
{
   int i = 0;
   for (i = 0; i < arraySize; i++)
   {
      if (strcmp(array[i], value) == 0)
      {
         strncpy(array[i], "EMPTY", maxChar);
      }
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
void GetRandomElementFrom2D(int maxChar, char array[][maxChar], int arraySize, int randomIncrement, char *returnValue)
{
   int randomNumber = GenerateRandomNumber(0, arraySize - 1, randomIncrement);

   int i = 0;
   while (strcmp(array[randomNumber], "EMPTY") == 0)
   {
      i++;
      randomNumber = GenerateRandomNumber(0, arraySize - 1, i);
   }

   strncpy(returnValue, array[randomNumber], 80);
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
   int randomNumber = GenerateRandomNumber(0, arraySize - 1, randomIncrement);
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
