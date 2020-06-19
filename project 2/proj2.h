/*
* Proj2.h
*/

#ifdef _PROJ2_H
#define _PROJ2_H

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/cred.h>
#include <linux/types.h>
//#include <linux/hashtable.h>
#include <linux/mutex.h>
/*
struct sysTable sysTable;
sysTable getTableMain(unsigned long);
void initalizeHash();
extern int findder(int, unsigned long);
*/

#define SIZE 32 //Size set in inialization

typedef struct sysTable{ //Hash table for systemcalls
	//int proc = -1; //PID
	unsigned long call; //Systemcall
	unsigned long count;
	int hash[SIZE]; //Manual hash table for proc id

	
	//struct hlist_node x_hash_list; //Linker
	//struct hlist_head *h_hash_list;
} sysTable;

static struct sysTable permList[437] = {}; //Array for blocking the potential 0-436 syscalls
DEFINE_MUTEX(lockTable); //General Lock
int started = 0; //Check if inialized

extern sysTable getTableMain();
int findder(int, unsigned long);
void initalizeHash(void);
int getHashs(int);
/*
extern int findder(int, unsigned long);
sysTable getTableMain(unsigned long);
void initalizeHash(void);
int getHash(int);
*/

#endif
