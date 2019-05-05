#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define START_ROOM      0
#define MID_ROOM        1
#define END_ROOM        2

struct Room 
{
  char* name;
  int numConnections;
  struct Room* connections[6];
  int type;
};

void makeDirectory(char* dir) {

  sprintf(dir, "sturtzj.rooms.%d", getpid());
  printf("Dir: %s", dir);
  mkdir(dir, S_IRUSR | S_IWUSR | S_IXUSR);
}

void initRooms(struct Room rooms[7]) {
  int i, j;

  for (i = 0; i < 7; i++) {
    char* name = calloc(9, sizeof(char));
    rooms[i].name = name;
    rooms[i].numConnections = 0;
    rooms[i].type = -1;
    for (j = 0; j < 6; j++) {
      rooms[i].connections[j] = NULL;
    }
  }
}

void deleteRooms(struct Room rooms[7]) {
  int i;  
  for (i = 0; i < 7; i++) {
    free(rooms[i].name);
  }
}

/* assigns START_ROOM, END_ROOM, and MID_ROOM types to the seven rooms */
void assignTypes(struct Room rooms[7]) {

  rooms[0].type = START_ROOM;
  rooms[6].type = END_ROOM;
  
  int i;
  for (i = 1; i < 7; i++) {
    rooms[i].type = MID_ROOM;
  }
}

/* assigns unique random names to each room from the roomNames argument */
void assignNames(struct Room rooms[7], char* roomNames[10]) {
  
  int i = 0; 
  while (i < 7) {
    int index = rand() % 10;
    if (roomNames[index] != NULL) {
      strcpy(rooms[i].name, roomNames[index]);
      roomNames[index] = NULL;
      i++;
    }
  }
}

/* Returns true if all rooms have 3 to 6 outbound connections, false otherwise */
int IsGraphFull(struct Room rooms[7])  
{
  int i;
  for (i = 0; i < 7; i++) 
  {
    if (rooms[i].numConnections < 3)
      return 0;
  }
  return 1;
}

/* Returns a random Room, does NOT validate if connection can be added */
struct Room* GetRandomRoom(struct Room rooms[7])
{
  int i = rand() % 7;
  return &rooms[i];
}

/* Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise */
int CanAddConnectionFrom(struct Room* x) 
{
  return x->numConnections < 6;
}

/* Returns true if a connection from Room x to Room y already exists, false otherwise */
int ConnectionAlreadyExists(struct Room* x, struct Room* y)
{
  /* ... */
}

/* Connects Rooms x and y together, does not check if this connection is valid */
void ConnectRoom(struct Room* x, struct Room* y) 
{
  /* ... */
}

/* Returns true if Rooms x and y are the same Room, false otherwise */
int IsSameRoom(struct Room* x, struct Room* y) 
{
  /* ... */
}

/* Adds a random, valid outbound connection from a Room to another Room */
void AddRandomConnection(struct Room rooms[7])  
{
  struct Room* A;   
  struct Room* B;

  while(1)
  {
    A = GetRandomRoom(rooms);

    if (CanAddConnectionFrom(A) == 1)
      break;
  }

  do
  {
    B = GetRandomRoom(rooms);
  }
  while(CanAddConnectionFrom(B) == 0 || IsSameRoom(A, B) == 1 || ConnectionAlreadyExists(A, B) == 1);

  ConnectRoom(A, B);  /*TODO: Add this connection to the real variables,*/
  ConnectRoom(B, A);  /*because this A and B will be destroyed when this function terminates*/
}

int main() {
  
  srand(time(0));

  char dir[20];
  memset(dir, '\0', 20);
  makeDirectory(dir);
  
  struct Room rooms[7];

  char* roomNames[10] = {
    "Socrates", 
    "Camus", 
    "Sartre", 
    "Marx", 
    "Proudhon", 
    "Bakunin", 
    "Soren K", 
    "Hegel", 
    "Cohen", 
    "Chomsky"
  };

  printf("testing roomnames: %s\n", roomNames[0]);
  
  initRooms(rooms);
  assignNames(rooms, roomNames);
  assignTypes(rooms);

  int test;
  test = IsGraphFull(rooms);
  printf("Testing IsGraphFull, should be 0: %d\n", test);
  
  int i;
  for (i = 0; i < 7; i++) {
    rooms[i].numConnections = 3;
  }
  
  char* testname = "This is a test";
  rooms[1].name = testname;

  rooms[0].connections[0] = &rooms[1];
  rooms[0].connections[1] = &rooms[2];
  rooms[0].connections[2] = &rooms[3];
  rooms[1].connections[0] = &rooms[2];
  rooms[1].connections[1] = &rooms[3];
  rooms[1].connections[2] = &rooms[4];
  rooms[2].connections[0] = &rooms[1];
  rooms[2].connections[1] = &rooms[3];
  rooms[2].connections[2] = &rooms[4];
  rooms[3].connections[0] = &rooms[2];
  rooms[3].connections[1] = &rooms[1];
  rooms[3].connections[2] = &rooms[4];
  rooms[4].connections[0] = &rooms[2];
  rooms[4].connections[1] = &rooms[3];
  rooms[4].connections[2] = &rooms[5];
  rooms[5].connections[0] = &rooms[2];
  rooms[5].connections[1] = &rooms[3];
  rooms[5].connections[2] = &rooms[4];
  rooms[6].connections[0] = &rooms[2];
  rooms[6].connections[1] = &rooms[3];
  rooms[6].connections[2] = &rooms[4];
  

  test = IsGraphFull(rooms);
  printf("Testing IsGraphFull, should be 1: %d\n", test);

  printf("Testing connection O->1\n");
  printf("Name of 0->1: %s\n", rooms[0].connections[0]->name);

  printf("Testing assignment of names\n");
  for (i = 0; i < 7; i++)
    printf("%s\n", rooms[i].name);

  printf("Testing getRandomRoom. Printing random names from func:\n");
  for (i = 0; i < 7; i++) {
    struct Room* next = GetRandomRoom(rooms);
    printf("%s\n", next->name);
  }

  printf("Testing CanAddConnection...\n");
  rooms[0].numConnections = 7;
  printf("Should be 0: %d\n", CanAddConnectionFrom(&rooms[0]));

  rooms[1].numConnections = 3;
  printf("Should be 1: %d\n", CanAddConnectionFrom(&rooms[1]));


  

  /* while (IsGraphFull(rooms) == 0) */
  /* { */
  /*   AddRandomConnection(); */
  /* } */

  /* char** randomNames[7] = malloc(7 * sizeof(char*)); */

  /* char* randomNames[7]; */
  /* randomNames = GetRandom(roomNames, 7); */

  /* printf("Process ID: %d\n", pid); */
  /* FILE* file = fopen("newdir/test.txt", "w"); */
  /* fprintf(file, "This is the new shit\n"); */
  /* fprintf(file, "This is the second new shit\n"); */
  /* fprintf(file, "This is the overridden shit\n"); */
  /* fclose(file); */
    
  /* char file[] = "newdir/test.txt"; */
  /* int file_descriptor = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); */
  return 0;
}

/* void testingSomeShit() { */

/*   char file[] = "testFile.txt"; */
/*   int file_descriptor; */

/*   file_descriptor = open(file, O_WRONLY | O_TRUNC); */
/*   if (file_descriptor < 0) { */
/*     fprintf(stderr, "Weird problem bro\n"); */
/*   } */
/*   else */ 
/*     printf("This shit will open up a file for writing and delete its existing contents brah. It\'s bitwise ORing the shit out of those bits brah\n"); */

/*   close(file_descriptor); */

/*   file_descriptor = open(file, O_WRONLY | O_APPEND); */
/*   if (file_descriptor < 0) { */
/*     fprintf(stderr, "Weird problem bro\n"); */
/*   } */
/*   else */ 
/*     printf("Opens for writing by appending to file\n") */

/*   close(file_descriptor); */

/*   char noFile[] = "notExistFile.txt"; */
/*   file_descriptor = open(noFile, O_WRONLY | O_CREAT, 0600); */
/*   // the O_CREAT tells C to create a file if it doesn't exist, but the file */
/*   // must also be given the right permissions, which is that number at the */ 
/*   // end which is some OCTAL shit that figures out your permissions */
  
/*   // alternatively: */ 
/*   file_descriptor = open(noFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); */

/*   // lseek shit */
/*   newpos = lseek(file_descriptor, 16, SEEK_SET); // moves 16 bytes from start */
/*   newpos = lseek(file_descriptor, 4, SEEK_CUR); // moves 4 bytes from currentpos */
/*   newpos = lseek(file_descriptor, 4, SEEK_END); // moves 4 bytes from endFile */


/*   // read() shit */
/*   int buffer[100];      // create buffer to get input from file */
/*   read(file_descriptor, &buffer, 10);   // reads ten bytes from file into buffer */
/*   write(file_descriptor, &buffer, 4);   // writes four bytes FROM buffer into f */
/* } */
