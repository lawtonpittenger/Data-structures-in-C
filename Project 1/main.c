/* COP 3502C Assignment 1
 * This program is written by: Lawton Pittenger */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Constant declarations
#define MAX_SIZE 50
#define INFILE "in.txt"
#define OUTFILE "out.txt"

// Structures declarations
typedef struct Monster {
    char *name;
    char *element;
    int population;
} Monster;

typedef struct Region {
    char *name;
    int nMonsters;
    int total_population;
    Monster **monsters;
} Region;

typedef struct Itinerary {
    int nRegions;
    Region **regions;
    int captures;
} Itinerary;

typedef struct Trainer {
    char *name;
    Itinerary *visits;
} Trainer;

// Created my own rounding function so I didn't have to use the math library.
int roundUp (float f) {
    if ((float) (f - (int)f) < 0.5f) return ((int)f);
    else return ((int) f + 1);
}

// Function that loads the file, and accounts for any missing / empty files.
FILE *loadFile (int *fileExists, char* name) {
    FILE *file_ptr;
        if (!strcmp (name, INFILE)) file_ptr = fopen (name, "r");
    else file_ptr = fopen (name, "w");
        if (file_ptr == NULL && !strcmp (name, INFILE)) {
        *fileExists = 0;
        printf("The file was not found! Please make sure you have selected correct file and try again.");
        file_ptr = fopen(name, "w");
    }
    else if (!strcmp (name, INFILE)) {
        if (fgetc (file_ptr) == EOF) {
            printf ("The \"%s\" file is empty! Please populate the file with the info and run again.\n", name);
            *fileExists = 0;
        }
        else fseek (file_ptr, 0, SEEK_SET);
    }
    return file_ptr;
}

// Function that creates the Monsters
Monster *createMonster (char *name, char *element, int population) {
//Allocates memory for monster
    Monster *monster = (Monster*) malloc (sizeof (Monster));
    monster->name = strdup (name);
    monster->element = strdup (element);
    monster->population = population;

    return monster;
}
// Function that reads the monsters
Monster **readMonsters (FILE *infile, int mCount) {
//Allocates memory for each monster
    Monster **monsters = (Monster**) malloc (sizeof (Monster*) * mCount);
    char name [MAX_SIZE], element [MAX_SIZE];
    int population;
    for (int i = 0; i < mCount; i++) {
        fscanf (infile, "%s %s %d", name, element, &population);
        monsters[i] = createMonster (name, element, population);
    }
    return monsters;
}
// Function that frees the monsters allocated memory
void freeMonsters (Monster** monsters, int mCount) {
    for (int i = 0; i < mCount; i++) free (monsters[i]);
    free (monsters);
}

// Function that creates Region 
Region *createRegion (char *name, int nMonsters, int total_population, Monster** monsters) {
    Region *region = (Region*) malloc (sizeof (Region));
    region->name = strdup (name);
    region->nMonsters = nMonsters;
    region->total_population = total_population;
    region->monsters = monsters;
    return region;
}
// Function that reads Regions
Region **readRegions (FILE *infile, int mCount, int rCount, Monster** monsters) {
// Allocates memory for each region
    Region **regions = (Region**) malloc (sizeof (Region*));
    char regionName [MAX_SIZE], monsterName [MAX_SIZE];
    int nMonsters, total_population;
    Monster **regMonsters = NULL;
    for (int i = 0; i < rCount; i++) {
        total_population = 0;
        fscanf (infile, "%s %d %*s",regionName, &nMonsters);
        regMonsters = (Monster**) malloc (sizeof (Monster*) * mCount);
        for (int j = 0; j < nMonsters; j++) {
            fscanf (infile, "%s", monsterName);
            for (int k = 0; k < mCount; k++) if (strcmp (monsterName, monsters[k]->name) == 0) {
                    regMonsters[j] = monsters[k];
                    total_population += monsters[k]->population;
                }
        }
        regions [i] = createRegion (regionName, nMonsters, total_population, regMonsters);
    }
    return regions;
}
// Function that frees regions allocated memory
void freeRegions (Region** regions, int rCount) {
    for (int i = 0; i < rCount; i++) {
        free (regions[i]->monsters);
        free (regions[i]);
    }
    free (regions);
}

// Function that creates Itinerary
Itinerary *createItinerary (int nRegions, Region** regions, int captures) {
    Itinerary *itinerary = (Itinerary*) malloc (sizeof (Itinerary));
    itinerary->nRegions = nRegions;
    itinerary->regions = regions;
    itinerary->captures = captures;

    return itinerary;
}
// Function that frees itinerarys allocated memory
void freeItinerary (Itinerary *itinerary) {
    free (itinerary);
}

// Function that creates trainer
Trainer *createTrainer (char *name, Itinerary *visits) {
    Trainer *trainer = (Trainer*) malloc (sizeof (Trainer));
    trainer->name = strdup (name);
    trainer->visits = visits;

    return trainer;
}
// Function that reads trainer
Trainer **readTrainers (FILE *infile, Region** regionList, int rCount, int numTrainers) {
    int numCaptures, nRegions;
    char currRegion [MAX_SIZE], currName [MAX_SIZE];
    Region ** regions = NULL;
    Itinerary *itinerary = NULL;
    Trainer **trainers = (Trainer**) malloc (sizeof (Trainer*) * numTrainers);

    for (int i = 0; i < numTrainers; i++) {
        fscanf (infile, "%s %d %*s %d %*s", currName, &numCaptures, &nRegions);
        regions = (Region**) malloc (sizeof (Region *) * nRegions);
        for (int j = 0, k = 0, fNEXT = 1; j < rCount && k < nRegions; j++) {
            if (fNEXT) {
                fscanf (infile, "%s", currRegion);
                fNEXT = 0;
            }
            if (strcmp (currRegion, regionList[j]->name) == 0) {
                fNEXT = 1;
                regions [k++] = regionList [j];
            }
        }
        itinerary = createItinerary (nRegions, regions, numCaptures);
        trainers [i] = createTrainer (currName, itinerary);
    }
    return trainers;
}
// Function that frees trainer allocated memory
void freeTrainers (Trainer **trainers, int numTrainers) {
    for (int i = 0; i < numTrainers; i++) {
        free (trainers[i]->visits->regions);
        freeItinerary (trainers[i]->visits);
        free (trainers[i]);
    }
    free (trainers);
}
// Function that frees all memory by calling all other free memory functions
void freeAllMemory (Trainer **trainers, Region **regions, Monster **monsters, int tCount, int rCount, int mCount) {
    freeTrainers (trainers, tCount);
    freeRegions (regions, rCount);
    freeMonsters (monsters, mCount);
}

// Main method begins here
int main() {
//Call atexit to test for mem leaks


// Code that checks if file exists / creates neccessary pointers.
    int fileExists = 1, *fPtr = &fileExists;
    FILE* file_ptr = loadFile (fPtr, INFILE);
    if (!fileExists) {
        return 1;
    }

//fscanf's through the file to grab neccessary information and store in corresponding variables
    int mCount, rCount, tCount,currentAverage = 0;
    fscanf (file_ptr, "%d %*s", &mCount); 
    Monster ** monsters = readMonsters (file_ptr, mCount);
    fscanf (file_ptr, "%d %*s", &rCount);
    Region ** regions = readRegions (file_ptr, mCount, rCount, monsters);
    fscanf (file_ptr, "%d %*s", &tCount);
    Trainer ** trainers = readTrainers(file_ptr, regions, rCount, tCount);

    // Close the input file, and load the output file to write too.
    fclose (file_ptr);
    loadFile (&fileExists, OUTFILE);

//For loops that go through and print to the outfile, first loop prints names of trainer
        for (int i = 0; i < tCount; i++) {
            fprintf(file_ptr, "%s\n", trainers[i]->name);

//Second for loop goes through and prints the name of each Region

        for (int j = 0; j < trainers[i]->visits->nRegions; j++) {
            fprintf (file_ptr, "%s\n", trainers[i]->visits->regions[j]->name);

//Third for loop goes through and prints the number of monsters captured per region, as well as the name of the monster.            
        for (int k = 0; k < trainers[i]->visits->regions[j]->nMonsters; k++) {
//Calculates current average, and rounds up the values to the next int.
                currentAverage = roundUp ((float) trainers[i]->visits->regions[j]->monsters[k]->population / (float) trainers[i]->visits->regions[j]->total_population * (float) trainers [i]->visits->captures);
                if (currentAverage){
                  fprintf (file_ptr, "%d %s\n", currentAverage, trainers[i]->visits->regions[j]->monsters[k]->name);
                } 
            }
        } 
        fprintf(file_ptr, "\n");
    }
//Close the file to ensure what we wrote to it gets saved
    fclose (file_ptr);

//Free all the allocated memory to prevent any memory leaks
    freeAllMemory (trainers, regions, monsters, tCount, rCount, mCount);
    return 0;

//And we're done
}

