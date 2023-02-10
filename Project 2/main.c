#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "leak_detector_c.h"

// Constant declarations
#define INFILE "in.txt"
#define OUTFILE "out.txt"
#define MAXSIZE 10
#define NAMESIZE 1000

// Struct declarations
typedef struct soldier {
    int index, sequenceNumber;
    struct soldier *next, *prev;
} Soldier;

typedef struct queue {
    Soldier *first, *last;
    char *name;
    int soldiers, k, th;
} Queue;

// Function declarations
Soldier *createSoldier (int sequence) {
    Soldier* new = (Soldier*) malloc (sizeof (Soldier));
    new->sequenceNumber = sequence;
    new->next = new->prev = NULL;
    return new;
}

// Function that checks if the queue is empty
int isEmpty (Queue *q) {
    return !q->first;
}

// Returns first solider, does not manipulate the data in any way, just returns the value.
int peek (Queue *q) {
    if (isEmpty (q)) return 0;
    else return q->first->index;
}

// Function that displays the queue
void display (Queue *q, FILE *filePtr) {
// First, check if the queue is empty
    if (!isEmpty(q)) {;
        fprintf (filePtr, "%d %s", q->first->sequenceNumber, q->name);
        for (int i = 0; i < q->soldiers; i++) {
            fprintf (filePtr, " %d", peek (q));
            q->first = q->first->next;
        }
        fprintf (filePtr, "\n");
    }
}

// Function to sort the queue to prep for elimination
void sortQueue (Queue **q, int numGroups) {
    Queue *tq;
    int fValid = 0, i = 0;
    while (!fValid) {
        if (i < numGroups) {
            if (q[i]->first->sequenceNumber > q[i + 1]->first->sequenceNumber) {
                tq = q[i];
                q[i] = q[i + 1];
                q[i + 1] = tq;
                i = -1;
            }
        }
        if (++i == numGroups - 1) fValid = 1;
    }
}

// Sorting algorithm to sort the specific soldier queues
void sortSoldierQueue (Queue **q, int numGroups) {
    Queue *tq;
    int fValid = 0, i = 0;
    while (!fValid) {
        if (i < numGroups) {
            if (q[i]->soldiers < q[i + 1]->soldiers) {
                tq = q[i];
                q[i] = q[i + 1];
                q[i + 1] = tq;
                i = -1;
            }

// To account for when two soldiers tie.
            else if (q[i]->soldiers == q[i + 1]->soldiers) {
                if (q[i]->first->sequenceNumber > q[i + 1]->first->sequenceNumber) {
                    tq = q[i];
                    q[i] = q[i + 1];
                    q[i + 1] = tq;
                    i = -1;
                }}}
        if (++i == numGroups - 1) fValid = 1;
    }
}

// Function that rearranges the linked list 
void rearrangeCircle(Queue *q) {
    // Check to see if list of soldiers is empty
    if (!isEmpty(q) && q->soldiers > 1) {
    // We need a current and a temp soldier to hold values.
        Soldier *current = q->first;
        Soldier *temp;
    // Do while loop so that it gets the first node in the list as well
        do {
            temp = current->prev;
            current->prev = current->next;
            current->next = temp;
            current = current->next;
        } while (current != q->first);
        q->first = q->first->next;
    }
}

// I combined enqueue and the create reverse circle into one function for efficiency purposes 
Queue *enqueue (int sequenceNumber, int soldiers) {
    Queue *q = (Queue*) malloc (sizeof (Queue));
    q->first = q->last = NULL;
    Soldier *current;
    q->soldiers = soldiers;

    for (int i = soldiers; i > 0; i--) {
        // Create a new soldier node
        current = createSoldier(sequenceNumber);
        current->index = i;
        // If it is the first value, we need to set the first and the last equal to be same  
        if (q->first == q->last && q->first == NULL) {
            q->first = q->last = current;
            q->first->next = q->last->next = q->first->prev = q->last->prev = NULL;
        }
        // Else, we set the next value to the current value, so that last->next never becomes NULL
        else {
            q->last->next = current;
            current->prev = q->last;
            q->last = current;
            q->last->next = q->first;
            q->first->prev = q->last;
        }
    }
    return q;
}


 // Function that deletes nodes from the queue
void killSoldier (Queue *q) {
   
    if (!isEmpty(q)) {
        Soldier *killed = q->first, *temp, *toDelete = q->first;
        temp = killed->prev;
        killed = killed->next;
        killed->prev = temp;
        temp->next = killed;
        free (toDelete);
    }
}

// Simple dequeue function to delete the first node in the list
void dequeue (Queue *q) {
    if (q->first != q->first->next) {
        killSoldier(q);
        q->first = q->first->next;
    }
}

// Phase 1 function
void phase1 (Queue **q, int numGroups, FILE *file) {
    int currSoldiers;
    fprintf (file, "\nPhase1 execution\n");
    for (int i = 0, j = 0; i < numGroups; i++) {
        currSoldiers = q[i]->soldiers;
        fprintf (file, "\nLine# %d %s\n", q[i]->first->sequenceNumber, q[i]->name);
        while (currSoldiers != q[i]->th) {
            if (j == q[i]->k - 1) {
                j = -1;
                currSoldiers--;
                fprintf(file, "Soldier# %d executed\n", q[i]->first->index);
                killSoldier (q[i]);
            }
            q[i]->first = q[i]->first->next;

            ++j;
        }
        j = 0;
    }
}

// Phase 2 function
void phase2 (Queue **q, int numGroups, FILE *file) {
    fprintf (file, "\nPhase2 execution\n");
    for (int i = 0; i < numGroups; i++) {
        if (q[i]->first->index != q[i]->first->next->index) {
            while (q[i]->first != q[i]->first->next) {
                fprintf(file, "Executed Soldier %d from line %d\n", q[i]->first->index, q[i]->first->sequenceNumber);
                dequeue(q[i]);
            }
        }
        if (i != numGroups - 1) {
            fprintf (file, "Executed Soldier %d from line %d\n", q[i]->first->index, q[i]->first->sequenceNumber);
            
        }
        else {
            fprintf (file, "\nSoldier %d from line %d will survive", peek (q[i]), q[i]->first->sequenceNumber);
        }
    }
}


int main() {
  //  atexit (report_mem_leak);
    FILE *file = fopen (INFILE, "r");

    // Check if input file excists
    if (file == NULL) {
        file = fopen (INFILE, "w");
        fclose (file);
        printf ("The file, \"%s\" was not found in the directory. Please populate the new file with your information.\n", INFILE);
        return 1;
    }
    // Declare the queue and numGroups variable
    Queue **queue;
    int numGroups;

    // Check if user is trying to create more groups that MAXSIZE
    if (fscanf (file, "%d", &numGroups) == 1) {
        if (numGroups > MAXSIZE) {
            printf("An invalid number of groups was declared in \"%s\": %d. A MAXSIZEimum of %d groups can be declared.\n",
                   INFILE, numGroups, MAXSIZE);
            fclose(file);
            return 2;
        }
    }


    // Allocates memory for proper number of groups
    queue = (Queue**) malloc (sizeof (Queue*) * numGroups);

    char name [NAMESIZE];
    int group, soldiers, k, th;
    // Iterates through the infile and adds the data to the queue struct
    for (int i = 0; i < numGroups; i++) {
        fscanf (file, "%d %s %d %d %d", &group, name, &soldiers, &k, &th);
            queue[i] = enqueue (group, soldiers);
            queue[i]->name = strdup (name);
            queue[i]->th = th;
            queue[i]->k = k;
        }
    // Sort the queue
    sortQueue (queue, numGroups);
    fclose (file);
    // Open up out file for writing output too
    file = fopen (OUTFILE, "w");
    fprintf (file, "Initial nonempty lists status\n");

    for (int i = 0; i < numGroups; i++) display (queue [i], file);
        fprintf (file, "\nAfter ordering nonempty lists status\n");
    for (int i = 0; i < numGroups; i++) {
        rearrangeCircle (queue[i]);
        display (queue[i], file);
    }
    // Call phase 1 function
    phase1 (queue, numGroups, file);
    sortSoldierQueue (queue, numGroups);
    for (int i = 0; i < numGroups; i ++) {
        // The list now needs soldiers to be formatted by their respective original number.
        for (int j = 0; j < queue[i]->soldiers; j++) {
            if (peek (queue [i]) > queue[i]->first->next->index) {
                queue[i]->first = queue[i]->first->next;
                j = -1;
            }
        }
    }
    // Call phase 2 function
    phase2 (queue, numGroups, file);
    // Loops that print the neccessary information for phase 2
    for (int i = 0; i < numGroups; i++) {
        while (queue[i]->first != queue[i]->first->next) {
            queue[i]->first = queue[i]->first->next;
            free (queue[i]->first->prev);
        }
      // Free all the allocated memory as phase 2 is now complete
        free (queue[i]->first);
        free (queue[i]);
    }
    //Finish freeing the memory
    free (queue);
    // Close the outfile so our changes are saved
    fclose (file);
    return 0;
}
    // And we're done !
