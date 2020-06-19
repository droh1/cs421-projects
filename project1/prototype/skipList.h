/*
* skipList.h
*
*/
#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h> //To get time
#include <linux/kernel.h>

//Globals
static unsigned int next_random = 9001;
int MAX_HEIGHT;

/*
* Struct for skiplist
*/
typedef struct slist{
	int maxHeight;
	int chance;
	struct node *start; 
}slist;

/*
* Nodes of skiplist
*/
typedef struct node{
	int key;
	int height;
	int uid;
	int start;	
	struct node **next;
	struct mailBox *mail;
}node;

/*
* Mailboxes
*/
typedef struct mailBox{
	char * msg;
	struct mailBox *nextMail;
}mailBox;

/*
* Generate random value for size of skiplist
*/
static unsigned int get_random_int(void);

/*
* Generate random seed
*/
static void seed_random(unsigned int);

/*
* NO LONGER USED? 

static int makeHeight(void);
*/

/*
* Creates a mailbox
*/
mailBox *createMailBox(Char);

/*
* Creates a skiplist
*/
slist *createSkipList(int, int);

/*
* Creates a node 
*/
slist *createNode(int, int, int, int);

/*
* Deletes the entire list
*/
void deleteSkip(slist *);

/*
* Searches list for key
*/
node *skipSearch(slist*, int);

/*
* Calculates chance a node gaines another pointer
*/
//int upHeight(slist);

/*
* Insert key
*/
//void insertKey(slist, int, int);

/*
* Delete key
*/
void deleteKey(slist*, int, int);

#endif


