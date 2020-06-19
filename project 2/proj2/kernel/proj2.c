/*
* Project 2 
*/

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/cred.h>
#include <linux/types.h>
//#include <linux/hashtable.h>
#include <linux/mutex.h>
#include <linux/proj2.h>
/*
#define SIZE 32 //Size set in inialization
int started = 0; //Check if inialized

typedef struct sysTable{ //Hash table for systemcalls
	//int proc = -1; //PID
	unsigned long call; //Systemcall
	unsigned long count;
	int hash[SIZE]; //Manual hash table for proc id

	
	//struct hlist_node x_hash_list; //Linker
	//struct hlist_head *h_hash_list;
} sysTable;

struct sysTable permList[437] = {}; //Array for blocking the potential 0-436 syscalls
DEFINE_MUTEX(lockTable); //General Lock

//Function Prototypes
int findder(int, unsigned long);
sysTable getTableMain(unsigned long);
void initalizeHash(void);
int getHash(int);
*/

/*
* Block pid from using syscall nr
*/
SYSCALL_DEFINE2 (sbx421_block, pid_t, proc, unsigned long, nr){
	if(started == 0){ //Check if initlized
		initalizeHash();
		started = 1;
	}else{
		sysTable *permList = getTableMain();
	}
	
	if(current_cred()->uid.val == proc){ //If the pid is blocking itself
		int index = getHashs(proc); //Get hash location
		mutex_lock(&lockTable); //Lock

		if(permList[nr].call != 999 && permList[nr].hash[index] == proc){ //Check if uid is already blocked
			printk("Error: Proccess %d is already blocked for system call %ld\n", proc, nr);
			permList[nr].call = permList[nr].call + 1; //Update count
			mutex_unlock(&lockTable);
			return -EEXIST;
		}

		permList[nr].hash[index] = proc; //Add uid to block
		permList[nr].call = nr; //Store syscall for sanity check
		permList[nr].count = 0;

		mutex_unlock(&lockTable); //Unlock
	}
	else if(current_cred()->uid.val == 0){ //If pid is root
		int index = getHashs(proc); //Get hash location
		mutex_lock(&lockTable); //Lock

		if(permList[nr].call != 999 && permList[nr].hash[index] == proc){ //Check if uid is already blocked
			printk("Error: Proccess %d is already blocked for system call %ld\n", proc, nr);
			permList[nr].call = permList[nr].call + 1; //Update count
			mutex_unlock(&lockTable);
			return -EEXIST;
		}

		permList[nr].hash[index] = proc; //Add uid to block
		permList[nr].call = nr; //Store syscall for sanity check
		permList[nr].count = 0;
		
		//call block only for user pid
		mutex_unlock(&lockTable); //Unlock
	}
	else{ //If the pid is trying to block another pid but is not root
		printk("Error: Process needs to be root to block other PID's calls\n");
		return -EPERM;
	}
	return 0; 
}

/*
* Unblock the process proc from systemcall nr
*/
SYSCALL_DEFINE2(sbx421_unblock, pid_t, proc, unsigned long, nr){
	if(started == 0){ //Check if initlized
		initalizeHash();
		started = 1;
	}else{
		sysTable *permList = getTableMain();
	}

	if(current_cred()->uid.val == proc){ //If the pid is unblocking itself
		int index = getHashs(proc); //Get hash location
		mutex_lock(&lockTable); //Lock

		if(permList[nr].call == nr && permList[nr].hash[index] == proc){ //Check if uid is blocked
			printk("Proccess %d is now unblocked for system call %ld\n", proc, nr);
			permList[nr].call = 999; //reset block
			permList[nr].hash[index] = 0; //reset block
			mutex_unlock(&lockTable);
			return 0;
		}

		//call block only for user pid
		mutex_unlock(&lockTable); //Unlock
		printk("Error: Process %d is not blocked from using %ld\n", proc, nr);

	}
	else if(current_cred()->uid.val == 0){ //If pid is root
		int index = getHashs(proc); //Get hash location
		mutex_lock(&lockTable); //Lock
		
		if(permList[nr].call == nr && permList[nr].hash[index] == proc){ //Check if uid is blocked
			printk("Proccess %d is now unblocked for system call %ld\n", proc, nr);
			permList[nr].call = 999; //reset block
			permList[nr].hash[index] = 0; //reset block
			mutex_unlock(&lockTable);
			return 0;
		}

		//call block only for user pid
		mutex_unlock(&lockTable); //Unlock
		printk("Error: Process %d is not blocked from using %ld\n", proc, nr);
	}
	else{ //If the pid is trying to block another pid but is not root
		printk("Error: Process needs to be root to block other PID's calls\n");
		return -EPERM;
	}
	return -EEXIST; //Return error if not found
}

/*
*Return number of times a the process proc called the systemcall nr
*/
SYSCALL_DEFINE2(sbx421_count, pid_t, proc, unsigned long, nr){
	if(started == 0){ //Check if initlized
		initalizeHash();
		started = 1;
	}else{
		sysTable *permList = getTableMain();
	}
	
	if(current_cred()->uid.val == 0){ //Check for root and valid proc
		if(proc != 0 && proc > 0){ //Check if a uid was negative or root
			int index = getHashs(proc); //Get hash location
			mutex_lock(&lockTable); //Lock
			
			if(permList[nr].call == nr && permList[nr].hash[index] == proc){ //Check if uid is found
				int temp = permList[nr].count; //Store count before unlocking
				mutex_unlock(&lockTable); //unlock
				return temp;
			}

			mutex_unlock(&lockTable); //Unlock
			printk("Error: Proccess %d is not found with %ld\n", proc, nr);
			return -EEXIST;
		}
		else{ //If a invalid pid was entered
			printk("Error: Invalid process to check\n");
			return -EINVAL;
		}
	}
	else{
		printk("Error: Process needs to be root\n");
		return -EPERM;
	}
}

/*
* Function calls for common.c to use
*/
int findder(int uid, unsigned long nr){
	int index;

	index = getHash(uid); //Get hash location
	if(permList[nr].call == nr && permList[nr].hash[index] == uid){ //Check if uid is found
		permList[nr].count = permList[nr].count + 1; //Update counter
		printk("UID %d is blocked from using %ld\n", uid, nr);
		return 1;
	}
	
	printk("UID %d is not blocked from using %ld\n", uid, nr);
	return 0;
}

/*
* Initalize the array with empty hashtables
*/
void initalizeHash(void){
	int countm = 0;

	for(countm = 0; countm <= 436; countm++){ //Intalize hashtables
		permList[countm].call = 999; 
		permList[countm].count = 0;
		//DEFINE_HASHTABLE(hashed, 16); //Declare hashtable
		//permList[countm]->h_hash_list = hashed; //Add hashtable to the array
	}
}

int getHashs(int key){
	return key % SIZE;
}

