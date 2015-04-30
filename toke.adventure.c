/**************************************************************
 * *  Filename: toke.adventure.c 
 * *  Coded by: Kevin To
 * *  Purpose - Lets a user play a game where they spawn in a room
 * *            and have to navigate to a specific room to win.
 * *
 * ***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

// Constant Declarations
#define maxRoomNameLen 80
#define maxRoomConnections 6 

// Struct Declarations
struct Room 
{
   char roomName[maxRoomNameLen];
   char connections[maxRoomConnections][maxRoomNameLen];
   char roomType[maxRoomNameLen];
   int numOpenConnections;
   int totalRoomConnections;
};
struct stat st = {0};

// Function Declarations
void GenerateRoomsDirectory();
void GenerateAllRoomFiles();
int GenerateRandomNumber(const int minNumber, const int maxNumber, const int timeOffSet);
void GetRandomElement(char **array, int arraySize, int randomIncrement, char *returnValue);
void GetRandomElementFrom2D(int maxChar, char array[][maxChar], int arraySize, int randomIncrement, char *returnValue);
void RemoveElementByValue(char **array, int arraySize, char *value);
void GetRoomsDirName(char *returnValue, int maxLen);
void GenerateRoomConnections(struct Room rooms[], int numRooms, int maxChar, char roomNames[][maxChar]);
void InitializeRoomsArray(struct Room rooms[], int maxRoomNumber, int maxChar, char roomNames[][maxChar]);
int FindHighPriorityRoom(struct Room rooms[], int numRooms, char *highPriorityRoom);
void LinkRoomToGraph(struct Room rooms[], int numRooms, char *highPriorityRoom, int highPriorityRoomPos);
void ClearRooms(struct Room rooms[], int maxRoomNumber);
int ImproperConnections(struct Room rooms[], int maxRoomNumber);
void LoadRooms(struct Room rooms[], int maxRoomNumber);
void RemoveNewLineAndAddNullTerm(char *stringValue);
void InitializeEmptyRooms(struct Room rooms[], int maxRoomNumber);
void AddConnectionToRoom(struct Room rooms[], int roomPos, char *connToAdd);
void DisplayRoomsStruct(struct Room rooms[], int maxRoomNumber);
void ExecuteGameLoop(struct Room rooms[], int maxRoomNumber);
void GetStartRoom(struct Room rooms[], int maxRoomNumber, char *startRoomName);
void DisplayCurrentRoom(struct Room rooms[], int maxRoomNumber, char *startRoomName);
int IsValidRoom(struct Room rooms[], int maxRoomNumber, char *currRoomName, char *userInputRoomName);
int IsEndRoom(struct Room rooms[], int maxRoomNumber, char *userInputRoomName);
void OutputVictoryMessage(char *userStepsFileName, int numUserSteps);
void AddRoomToTrackingFile(char *userStepsFileName, char *currRoomName);

// Program entry point
int main()
{
   // Create all the room files
   GenerateRoomsDirectory();
   GenerateAllRoomFiles();

   // Load the room files into memory and play the game
   struct Room loadedRooms[7];
   InitializeEmptyRooms(loadedRooms, 7);
   LoadRooms(loadedRooms, 7);
   ExecuteGameLoop(loadedRooms, 7);

   exit(EXIT_SUCCESS);

   // This is a test method to output all the rooms struct
   // DisplayRoomsStruct(loadedRooms, 7);
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the max number of rooms
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Runs the game loop for the user.
 * *
 * ***************************************************************/
void ExecuteGameLoop(struct Room rooms[], int maxRoomNumber)
{
   // Get the start room
   char startRoomName[80];
   char currentRoomName[80];
   GetStartRoom(rooms, maxRoomNumber, startRoomName);
   strncpy(currentRoomName, startRoomName, 80);

   // Construct the tracker file name
   char directoryName[80];
   GetRoomsDirName(directoryName, 80);
   char userStepsFileName[80];
   strncpy(userStepsFileName, directoryName, 80);
   strcat(userStepsFileName, "/");
   strncat(userStepsFileName, "tracker", 80);

   int userWon = 0;
   int numUserSteps = 0;
   while (userWon == 0)
   {
      DisplayCurrentRoom(rooms, maxRoomNumber, currentRoomName);

      // Get user room name input
      char userInput[80];
      fgets(userInput, 80, stdin);
      printf("\n");
      RemoveNewLineAndAddNullTerm(userInput);

      // Check if the player made a valid move
      if (IsValidRoom(rooms, maxRoomNumber, currentRoomName, userInput) == 0)
      {
         continue;
      }

      // Set the current room to what the user entered
      strncpy(currentRoomName, userInput, 80);

      // Track the user's movements
      numUserSteps++;
      AddRoomToTrackingFile(userStepsFileName, currentRoomName);

      // Check if user arrived at the end room
      if (IsEndRoom(rooms, maxRoomNumber, userInput) == 1)
      {
         OutputVictoryMessage(userStepsFileName, numUserSteps);
         userWon = 1;
      }
   }

}

/**************************************************************
 * * Entry:
 * *  userStepsFileName - the file name of the steps tracker
 * *  currRoomName - the current room name
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Adds the current room name to the tracker file.
 * *
 * ***************************************************************/
void AddRoomToTrackingFile(char *userStepsFileName, char *currRoomName)
{
   FILE *filePointer;

   filePointer = fopen(userStepsFileName, "a");
   fprintf(filePointer, "%s\n", currRoomName);
   fclose(filePointer);
}

/**************************************************************
 * * Entry:
 * *  userStepsFileName - the file name of the steps tracker
 * *  numUserSteps - the number of steps the user took to victory
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Outputs the victory message.
 * *
 * ***************************************************************/
void OutputVictoryMessage(char *userStepsFileName, int numUserSteps)
{
   // Output victory message and number of steps
   printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
   printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", numUserSteps);

   // Print out the contents of the tracker file
   FILE *filePointer;
   filePointer = fopen(userStepsFileName, "r");
   if (filePointer) 
   {
      int character;
      while ((character = getc(filePointer)) != EOF)
      {
         putchar(character);
      }
      fclose(filePointer);

      // Clean up the temp tracker file
      remove(userStepsFileName);
   }
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the max number of rooms
 * *  userInputRoomName - room you want to check if end room
 * *
 * * Exit:
 * *  Returns 0, if room is not end room.
 * *  Returns 1, if room is end room. 
 * *
 * * Purpose:
 * *  Checks if the user inputed room is the end room.
 * *
 * ***************************************************************/
int IsEndRoom(struct Room rooms[], int maxRoomNumber, char *userInputRoomName)
{
   int i;
   for (i = 0; i < maxRoomNumber; i++)
   {
      if (strcmp(rooms[i].roomName, userInputRoomName) == 0 && strcmp(rooms[i].roomType, "END_ROOM") == 0)
      {
         return 1; // Return True, this is the end room.
      }
   }

   return 0; // Return False, this is not the end room.
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the max number of rooms
 * *  currRoomName - the current room name
 * *  userInputRoomName - the room the user wants to go to
 * *
 * * Exit:
 * *  Returns 0, if user inputed a invalid room.
 * *  Returns 1, if user inputed a valid room.
 * *
 * * Purpose:
 * *  Checks if the user inputed room is reach able from the current room.
 * *
 * ***************************************************************/
int IsValidRoom(struct Room rooms[], int maxRoomNumber, char *currRoomName, char *userInputRoomName)
{
   int i;
   int j;

   // Check if this is an actual room
   int roomExists = 0;
   for (i = 0; i < maxRoomNumber; i++)
   {
      if (strcmp(rooms[i].roomName, userInputRoomName) == 0)
      {
         roomExists = 1;
      }
   }

   // Check if user inputed room is reachable
   int connectionPossible = 0;
   for (i = 0; i < maxRoomNumber; i++)
   {
      if (strcmp(rooms[i].roomName, currRoomName) == 0)
      {
         for (j = 0; j < maxRoomConnections; j++)
         {
            if (strcmp(rooms[i].connections[j], userInputRoomName) == 0)
            {
               connectionPossible = 1;
               break;
            }
         }
      }
   }

   // Output an error message if:
   //   1. Room is not a real room.
   //   2. Room cannot be reached because it is not connected to the 
   //      current room.
   //   3. Room you are trying to reach is the current room.
   if (roomExists == 0 || connectionPossible == 0)
   {
      printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
      return 0; // Means false, this is a an invalid room.
   }
  
   return 1; // Means true, this is a valid room.
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the max number of rooms
 * *  roomName - the room name
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Displays the output for the current room that is user is in.
 * *
 * ***************************************************************/
void DisplayCurrentRoom(struct Room rooms[], int maxRoomNumber, char *roomName)
{
   // Loop through all the rooms
   int i;
   int j;
   for (i = 0; i < maxRoomNumber; i++)
   {
      // Output the specified room to the user
      if (strcmp(rooms[i].roomName, roomName) == 0)
      {
         printf("CURRENT LOCATION: %s\n", rooms[i].roomName);
         printf("POSSIBLE CONNECTIONS: ");

         // Output the formatted room names
         for (j = 0; j < rooms[i].totalRoomConnections; j++)
         {
            if (j == (rooms[i].totalRoomConnections - 1))
            {
               printf("%s.\n", rooms[i].connections[j]);
            }
            else
            {
               printf("%s, ", rooms[i].connections[j]);
            }

         }
      }
   }

   printf("WHERE TO?>");
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the max number of rooms
 * *  startRoomName - variable to hold the return value, which is 
 * *                  the starting room name
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Gets the starting room name
 * *
 * ***************************************************************/
void GetStartRoom(struct Room rooms[], int maxRoomNumber, char *startRoomName)
{
   int i;
   for (i = 0; i < maxRoomNumber; i++)
   {
      if (strcmp(rooms[i].roomType, "START_ROOM") == 0)
      {
         strncpy(startRoomName, rooms[i].roomName, 80);
      }
   }
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the max number of rooms
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Loads the rooms from the room's directory into the room's struct
 * *  array.
 * *
 * ***************************************************************/
void LoadRooms(struct Room rooms[], int maxRoomNumber)
{
   int i;
   char fileName[80];
   char directoryName[80];
   GetRoomsDirName(directoryName, 80);
   char roomNames[7][80];

   // Get all the room names and save to an array
   DIR *directoryPointer;
   directoryPointer = opendir(directoryName);
   if (directoryPointer)
   {
      struct dirent *dir;
      i = 0;
      while ((dir = readdir(directoryPointer)) != NULL)
      {
         if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
         {
            strncpy(roomNames[i], dir->d_name, 80);
            i++;
         }
      }
      closedir(directoryPointer);
   }

   char readString[200];
   char saveString[80];
   for (i = 0; i < maxRoomNumber; i++)
   {
      FILE *filePointer;
      // Generate the file name
      strncpy(fileName, directoryName, 80);
      strcat(fileName, "/");
      strncat(fileName, roomNames[i], 80);

      // Load the room name to the struct
      strncpy(rooms[i].roomName, roomNames[i], 80);

      // Read from files and load into room struct
      filePointer = fopen(fileName, "r");
      while(fgets(readString, 200, filePointer)) 
      {
         // Load the connections
         if (strstr(readString, "CONNECTION") != NULL) {
            strncpy(saveString, readString + 14, 80);
            RemoveNewLineAndAddNullTerm(saveString);
            AddConnectionToRoom(rooms, i, saveString);
         }

         // Load the room type
         if (strstr(readString, "ROOM TYPE:") != NULL) {
            strncpy(saveString, readString + 11, 80);
            RemoveNewLineAndAddNullTerm(saveString);
            strncpy(rooms[i].roomType, saveString, 80);
         }
      }

      fclose(filePointer);
   }
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the max number of rooms
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Displays all the rooms in the rooms struct
 * *
 * ***************************************************************/
void DisplayRoomsStruct(struct Room rooms[], int maxRoomNumber)
{
   // Print out all the objects
   int i;
   int j;
   for (i = 0; i < maxRoomNumber; i++)
   {
      printf("--------------room name: %s\n", rooms[i].roomName);
      printf("room type: %s\n", rooms[i].roomType);
      printf("open conn: %d\n", rooms[i].numOpenConnections);
      printf("total conn: %d\n", rooms[i].totalRoomConnections);

      for (j = 0; j < maxRoomConnections; j++)
      {
         printf("room connection %d: %s\n", j, rooms[i].connections[j]);
      }
   }
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  roomPos - the room position in the array
 * *  connToAdd - the room to connect to's name
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Adds a connection to a room for a room loading procedure.
 * *
 * ***************************************************************/
void AddConnectionToRoom(struct Room rooms[], int roomPos, char *connToAdd)
{
   int i;
   for (i = 0; i < 6; i++)
   {
      // Add the connection if an unused connection is found
      if (strcmp(rooms[roomPos].connections[i], "CLOSED") == 0)
      {
         strncpy(rooms[roomPos].connections[i], connToAdd, 80);
         rooms[roomPos].totalRoomConnections++;
         return;
      }
   }
}

/**************************************************************
 * * Entry:
 * *  stringValue - the string you want to transform
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Removes the new line character from the string and adds a null 
 * *  terminator in its place.
 * *
 * ***************************************************************/
void RemoveNewLineAndAddNullTerm(char *stringValue)
{
   size_t ln = strlen(stringValue) - 1;
   if (stringValue[ln] == '\n')
   {
      stringValue[ln] = '\0';
   }  
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber  - the number of rooms in the room struct
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  For the rooms struct. Makes room name and room type into empty
 * *  strings. Sets all int properties to 0. Sets all connections
 * *  to closed.
 * *
 * ***************************************************************/
void InitializeEmptyRooms(struct Room rooms[], int maxRoomNumber) 
{
   int i;
   int j;
   for (i = 0; i < maxRoomNumber; i++)
   {
      strncpy(rooms[i].roomName, "", 80);
      strncpy(rooms[i].roomType, "", 80);
      rooms[i].numOpenConnections = 0;
      rooms[i].totalRoomConnections = 0;

      for (j = 0; j < maxRoomConnections; j++)
      {
         strncpy(rooms[i].connections[j], "CLOSED", 80);
      }
   }
}

 /**************************************************************
 * * Entry:
 * *  n/a
 * *
 * * Exit:
 * *  No return value
 * *
 * * Purpose:
 * *  Creates the room directory
 * *
 * ***************************************************************/
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
}

 /**************************************************************
 * * Entry:
 * *  n/a
 * *
 * * Exit:
 * *  No return value
 * *
 * * Purpose:
 * *  Generates the room files
 * *
 * ***************************************************************/
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
   GenerateRoomConnections(rooms, maxRoomNumber, 80, roomNames);

   // Check if there are any nodes with less than 2 connections.
   // If there are, redo all the connections
   while (ImproperConnections(rooms, maxRoomNumber) == 1)
   {
      printf("Rooms were connected incorrectly. Regenerating room connections. Please wait...\n");
      ClearRooms(rooms, maxRoomNumber);
      InitializeRoomsArray(rooms, maxRoomNumber, 80, roomNames);
      GenerateRoomConnections(rooms, maxRoomNumber, 80, roomNames);  
   }

   // Create the files
   int j;
   for (i = 0; i < maxRoomNumber; i++)
   {
      // Generate the file name
      strncpy(fileName, directoryName, 80);
      strcat(fileName, "/");
      strncat(fileName, rooms[i].roomName, 80);
     
      // Create the files
      filePointer = fopen(fileName, "w");
      fprintf(filePointer, "ROOM NAME: %s\n", rooms[i].roomName);

      for (j = 0; j < maxRoomConnections; j++)
      {
         if (strcmp(rooms[i].connections[j], "OPEN") == 0 || strcmp(rooms[i].connections[j], "CLOSED") == 0)
         {
            continue;
         }

         fprintf(filePointer, "CONNECTION %d: %s\n", j + 1, rooms[i].connections[j]);
      }
      fprintf(filePointer, "ROOM TYPE: %s\n", rooms[i].roomType);

      fclose(filePointer);
   }
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber  - the number of rooms in the room struct
 * *
 * * Exit:
 * *  Return 1, if there are any rooms with incorrect connections.
 * *  Return 0, if otherwise.
 * *
 * * Purpose:
 * *  Checks if there are any rooms with less than 3 connections.
 * *
 * ***************************************************************/
int ImproperConnections(struct Room rooms[], int maxRoomNumber)
{
   int i;
   int numberOfTakenConnections = 0;
   for (i = 0; i < maxRoomNumber; i++)
   {
      numberOfTakenConnections = rooms[i].totalRoomConnections - rooms[i].numOpenConnections;
      if ((numberOfTakenConnections) < 3)
      {
         return 1;
      }
   }
   
   return 0;
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber  - the number of rooms in the room struct
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Clears out all of the info in each room struct. This effectively
 * *  resets all the rooms.
 * *
 * ***************************************************************/
void ClearRooms(struct Room rooms[], int maxRoomNumber)
{
   int i;
   int j;
   for (i = 0; i < maxRoomNumber; i++)
   {
      strncpy(rooms[i].roomName, "", 80);
      strncpy(rooms[i].roomType, "", 80);

      rooms[i].numOpenConnections = 0;
      rooms[i].totalRoomConnections = 0;

      for (j = 0; j < maxRoomConnections; j++)
      {
         strncpy(rooms[i].connections[j], "", 80);
      }
   }
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the number of rooms in the room struct
 * *  maxChar - max number of characters in each room name
 * *  roomNames - an array containing room names
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Sets a random start room and end room. Makes all other rooms mid
 * *  rooms. Also Determines how many rooms each room can connect to, randomly.
 * *
 * ***************************************************************/
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

   // Loop through all the rooms to set various properties 
   int randomNumber;
   int i;
   for (i = 0; i < maxRoomNumber; i++)
   {
      // Assign the room name
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
      rooms[i].numOpenConnections = randomNumber;
      rooms[i].totalRoomConnections = randomNumber;

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

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the number of rooms in the room struct
 * *  maxChar - max number of characters in each room name
 * *  roomNames - an array containing room names
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Create a link between all the rooms
 * *
 * ***************************************************************/
void GenerateRoomConnections(struct Room rooms[], int numRooms, int maxChar, char roomNames[][maxChar])
{
   // Connect all the rooms together. The order of rooms will be random in each run.
   // This first block of code will link all the rooms together, so that there will
   // be a path through all the rooms.
   int i;
   for (i = 0; i < numRooms; i++)
   {
      // Decrement the number tracking the current room's open connections
      // by two because we are creating a both through all rooms; which
      // require two connections.
      rooms[i].numOpenConnections -= 2;

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
   }

   // Connect any rooms with open connections to any available rooms
   int highPriorityRoomPos = 0;
   char highPriorityRoom[80];
   for (i = 0; i < 20; i++)
   {
      highPriorityRoomPos = FindHighPriorityRoom(rooms, numRooms, highPriorityRoom); // need to add more priorities
      LinkRoomToGraph(rooms, numRooms, highPriorityRoom, highPriorityRoomPos);
   }

   LinkRoomToGraph(rooms, numRooms, highPriorityRoom, highPriorityRoomPos);
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the number of rooms in the room struct
 * *  highPriorityRoom - room to link to other already linked rooms
 * *  highPriorityRoomPos - position of the room to link in the room 
 * *                        struct array
 * *
 * * Exit:
 * *  n/a
 * *
 * * Purpose:
 * *  Create a link between the specified room and a room that is 
 * *  already linked to other rooms. This ensure that there will 
 * *  not be isolated rooms
 * *
 * ***************************************************************/
void LinkRoomToGraph(struct Room rooms[], int numRooms, char *highPriorityRoom, int highPriorityRoomPos)
{
   int i;
   int j;
   char availableRoomName[80];
   int isAvailable = 1;
   int availRoomPos = -1;
   for (i = 0; i < numRooms; i++)
   {
      isAvailable = 1;

      // Go to next room if the current room has no open connections or is the same as 
      // the room we want to connect
      if (rooms[i].numOpenConnections == 0 || strcmp(rooms[i].roomName, highPriorityRoom) == 0)
      {
         continue;
      }

      // Check if the current room's room connections already contain the room we want to add.
      for (j = 0; j < maxRoomConnections; j++)
      {
         if (strcmp(rooms[i].connections[j], highPriorityRoom) == 0)
         {
            isAvailable = 0;
         }
      }

      // Check if current room is available to link to the specified room
      if (isAvailable == 1)
      {
         availRoomPos = i;
         strncpy(availableRoomName, rooms[i].roomName, 80);
         break;
      }
   }

   if (isAvailable == 1 && availRoomPos != -1)
   {
      // Connect available room to specified room
      for (i = 0; i < maxRoomConnections; i++)
      {
         if (strcmp(rooms[availRoomPos].connections[i], "OPEN") == 0)
         {
            rooms[availRoomPos].numOpenConnections--;
            strncpy(rooms[availRoomPos].connections[i], highPriorityRoom, 80);
            break;
         }
      }

      // Connect specified room to available room
      for (i = 0; i < maxRoomConnections; i++)
      {
         if (strcmp(rooms[highPriorityRoomPos].connections[i], "OPEN") == 0)
         {
            rooms[highPriorityRoomPos].numOpenConnections--;
            strncpy(rooms[highPriorityRoomPos].connections[i], availableRoomName, 80);
            break;
         }
      }
   }
}

/**************************************************************
 * * Entry:
 * *  rooms - an array of room structs
 * *  maxRoomNumber - the number of rooms in the room struct
 * *  highPriorityRoom - holds the value of the highest priority
 * *                     room to link
 * *
 * * Exit:
 * *  Returns an int value of the position of the high priority 
 * *  room in the rooms struct array
 * *
 * * Purpose:
 * *  To find the next room to link. 
 * *
 * ***************************************************************/
int FindHighPriorityRoom(struct Room rooms[], int numRooms, char *highPriorityRoom)
{
   int i;

   // Returns rooms with less than 3 connections
   for (i = 0; i < numRooms; i++)
   {
      if ((rooms[i].totalRoomConnections - rooms[i].numOpenConnections) < 3)
      {
         strcpy(highPriorityRoom, rooms[i].roomName);
         return i;
      }
   }

   // Returns rooms with any open connections
   for (i = 0; i < numRooms; i++)
   {
      if (rooms[i].numOpenConnections > 1)
      {
         strcpy(highPriorityRoom, rooms[i].roomName);
         return i;
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
