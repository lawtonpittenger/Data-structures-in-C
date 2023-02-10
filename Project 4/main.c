/* COP 3502C Programming Assignment 5
This program is written by: Lawton Pittenger */ 

//Include necessary libraries, define constants and structures.
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "leak_detector_c.h"
#define ALPHABET_SIZE 26
// Converts current character into index
// uses only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

// Trie node declaration
struct TrieNode
{
    struct TrieNode* children[ALPHABET_SIZE];
    int freq;
    int sum_freq; // sum of the frequencies for which the string is a prefix of words in the dictionary, including itself
    int cur_max_freq; // The maximum frequency of any child node.  It will be the sum_freq of the child node
};

    //Function templates

// Returns new trie node (initialized to NULLs) 
struct TrieNode* getNode(void);

// If not present, inserts key into trie. If the key is prefix of trie node, just marks leaf node
void insert(struct TrieNode* root, const char* key);

//This function is used to query for words
void query(struct TrieNode* root, char* word, char prediction[27]);

//This function traverses the tree in post order to free the memory of each and every node of the tree
void freeTree(struct TrieNode* root);

//This function is used to build the tree, by scanning in the items from in.txt.
void build_tree();

// Returns new trie node (initialized to NULLs) 
struct TrieNode* getNode(void)
{
    struct TrieNode* pNode = NULL;
    pNode = (struct TrieNode*)malloc(sizeof(struct TrieNode));

    pNode->cur_max_freq = 0;
    pNode->freq = 0;
    pNode->sum_freq = 0;

    if (pNode)
    {
        for (int i = 0; i < ALPHABET_SIZE; i++)
            pNode->children[i] = NULL;
    }
    return pNode;
}

/* Looks at the child nodes of the passed node and determines
   which one has the greatest frequency.  It returns that frequency value.
   It takes into account the character passed, because it will be inserted at
   this level */
int get_max_frequency_child(struct TrieNode* node,char chr)
{
    int max_frequency = 0;
    int add = 0;
    for (int i = 0; i < 26; i++) {
        if (node->children[i] != NULL) {
            add = 0;
            /* Since this character will be inserted into the child node,
              i.e. at the next level, it is added to the sum_freq */
            if (chr == (char)((int)i + (int)'a'))  add = 1; 
            if ((node->children[i]->sum_freq + add) > max_frequency) {
                max_frequency = node->children[i]->sum_freq + add;
            }
        }
    }
    /* If max_frequency is zero, make it a 1, because
       this is based on the idea that a character will be added at the next level */
    if (max_frequency == 0) {
        max_frequency = 1;
    }
    return max_frequency;
}

/* Looks at the child nodes of the passed node and determines
   which one has the greatest frequency.  It passes back the characters
   it represents in the character array, "prediction", in alphabetical order
   It is passed the max number so that it knows which to extract */
void max_frequency_child_chars(struct TrieNode* node,int max, char prediction[27])
{
    int max_frequency = 0;
    int slot = 0;
    int idx = 0;

    for (int i = 0; i < 26; i++) {
        if (node->children[i] != NULL) {
            if (node->children[i]->sum_freq == max) {
                prediction[idx++] = (char)(i + (int)'a');                
            }
        }
    }
    prediction[idx] = '\0';
}

/* Looks for the text passed in "word" in the dictionary passed to "root".
   It then predicts the next letters and passes them back in the "prediction"
   array. If the results are "unknown," it passes back an empty "prediction" string */
void query(struct TrieNode* root, char* word, char prediction[27])
{
    int level;
    int length = strlen(word);
    int index = 0;

    // initialize prediction to an empty string
    prediction[0] = '\0';

    //Create new TrieNode pointer and assign its value to root.
    struct TrieNode* pCrawl = root;

    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(word[level]);
        if (!pCrawl->children[index]) {           
            // unknown word
            prediction[0] = '\0';
            return;
        }
        pCrawl = pCrawl->children[index];

        //At last character, predict text
        if (level == length - 1) {
            if(pCrawl->cur_max_freq != 0)
                max_frequency_child_chars(pCrawl, pCrawl->cur_max_freq, prediction);
            else { // unknown word
                prediction[0] = '\0';
            }
            return;
        }
    }
}

// If not present, insert the key into the trie
// If the key is prefix of trie node, just marks leaf node 
void insert(struct TrieNode* root, const char* key)
{
    int level;
    int length = strlen(key);
    int index=0;

    struct TrieNode* pCrawl = root;

    for (level = 0; level < length; level++)
    {
         index = CHAR_TO_INDEX(key[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();
        pCrawl = pCrawl->children[index];

        /* At the last character of the word, cur_max_freq
          does not increase, since there are no characters
          added to the child in this insertion */
        if (level < length - 1)
            pCrawl->cur_max_freq = get_max_frequency_child(pCrawl, key[level + 1]);
        else //At the final character you add to the count for end of words and number of times it was entered
            pCrawl->freq++;

        /* This always increments when a word is inserted, since it's the sum of all words added, including
           this one and the ones added to the children */
        pCrawl->sum_freq++;
    }
}

 /* This function traverses the tree in post order to
 free the memory of each and every node of the tree
 Pass the root to delete the whole tree. */
void freeTree(struct TrieNode* root)
{
    int i = 0;
    if (root == NULL) return;
    
    //first free the subtrees
    for (i = 0; i < 26; i++) {
        freeTree(root->children[i]);
    }
    //then free the node
    free(root);
}
//Function that builds the tree
void build_tree()
{
    int i = 0, j=0;
    int numberOfCommands;
    int command, num;
    char word[100];
    char prediction[27]; // This holds the returned predicted text

    struct TrieNode* dictionary; // The dictionary

    FILE* fpIn = fopen("in.txt", "r");

    //Account for in.txt not being created.
    if (fpIn == NULL)
    {
        printf("in.txt failed to open\n");
        return;
    }
    //Account for out.txt not being created.
    FILE* fpOut = fopen("out.txt", "w");
    if (fpOut == NULL) {
        printf("out.txt failed to be opened\n");
    }

    // Create the dictionary root
    dictionary = getNode();
    fscanf(fpIn, "%d", &numberOfCommands);
    for (i = 0; i < numberOfCommands; i++) {
        fscanf(fpIn, "%d", &command);

        //Case 1: Insert a new word
        if (command == 1) {
            fscanf(fpIn, "%s %d", word, &num);
            // Insert the word num times into the dictionary
            for (j = 0; j < num; j++) {
                insert(dictionary, word);
            }
        }
        //Case 2: Query for a word
        else if (command == 2) {
            fscanf(fpIn, "%s", word);
            //query function goes here
            query(dictionary, word, prediction);
            if (prediction[0] == '\0') {
                fprintf(fpOut, "unknown word\n");
            }
            else {
                fprintf(fpOut, "%s\n", prediction);
            }
        }
    }
    //Close the in.txt and out.txt files
    fclose(fpIn);
    fclose(fpOut);

    //Use post order traversal to free the trie tree memory
    freeTree(dictionary);
}

//Nice and clean main method
int main()
{
    atexit(report_mem_leak);
    build_tree();
}




