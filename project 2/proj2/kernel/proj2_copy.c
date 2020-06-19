/*
* Project 2 
*/


//#include <linux/proj2.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/cred.h>
#include <linux/types.h>
#include <linux/hashtable.h>
#include <linux/mutex.h>


#define SIZE 15; //Size set in inialization
int started = 0; //Check if inialized

typedef struct sysTable{ //Hash table for systemcalls
	int proc; //PID
	unsigned long call; //Systemcall
	unsigned long count;
	//struct hlist_node x_hash_list; //Linker
	struct hlist_head *h_hash_list;
} sysTable;

//Array for blocking the potential 0-436 syscalls
sysTable *permList[437]; 
DEFINE_MUTEX(lockTable); //General Lock

//Function Prototypes
sysTable getTableMain(unsigned long);
void initalizeHash(void);
int findder(int, unsigned long);

/*
* Block pid from using syscall nr
*/
SYSCALL_DEFINE2 (sbx421_block, pid_t, proc, unsigned long, nr){
	if(started == 0){ //Check if initlized
		initalizeHash();
		started = 1;
	}
	
	if(current_cred()->uid.val == proc){ //If the pid is blocking itself
		mutex_lock(&lockTable); //Lock
		sysTable *check;
		sysTable *procHash = getTableMain(nr); //Get the hashtable from array 

		hash_for_each_possible(procHash, check, h_hash_list, proc){ //Check if pid with nr is already blocked
			if(check->proc == proc && check->call == nr){ //If found
				printk("Error: Proccess %d is already blocked for system call %ld\n", proc, nr);
				mutex_unlock(&lockTable);
				return -EEXIST;
			}
		} 

		sysTable *temp;
		temp = (sysTable*)kmalloc(sizeof(hashTable), GFP_KERNEL); //Allocate memeory

		if(temp == NULL){
			printk("Error: Unable to allocate memeory\n");
			return -ENOMEM;
		}

		temp->proc = proc;
		temp->call = nr;
		temp->count = 0;
		hash_add(procHash, &temp->h_hash_list, temp->proc); //Add to hash
		
		//call block only for user pid
		mutex_unlock(&lockTable); //Unlock
	}
	else if(current_cred()->uid.val == 0){ //If pid is root
		mutex_lock(&lockTable); //Lock
		
		sysTable *check;
		sysTable *procHash = getTableMain(nr); //Get the hashtable from array 
		
		hash_for_each_possible(procHash, check, h_hash_list, proc){ //Check if pid with nr is already blocked
			if(check->proc == proc && check->call == nr){ //If found
				printk("Error: Proccess %d is already blocked for system call %ld\n", proc, nr);
				mutex_unlock(&lockTable);
				return -EEXIST;
			}
		}

		sysTable *temp;
		temp = (sysTable*)kmalloc(sizeof(hashTable), GFP_KERNEL); //Allocate memeory

		if(temp == NULL){
			printk("Error: Unable to allocate memeory\n");
			return -ENOMEM;
		}

		temp->proc = proc;
		temp->call = nr;
		temp->count = 0;
		hash_add(procHash, &temp->h_hash_list, temp->proc); //Add to hash
		
		//call block only for user pid
		mutex_unlock(&lockTable); //Unlock
	}
	else{ //If the pid is trying to block another pid but is not root
		printk("Error: Process needs to be root to block other PID's calls\n");
		return -EPERM;
	}
	return -EPERM; //Wont reach this state but to stop compiler from complaining
}

/*
* Unblock the process proc from systemcall nr
*/
SYSCALL_DEFINE2(sbx421_unblock, pid_t, proc, unsigned long, nr){
	if(started == 0){ //Check if initlized
		initalizeHash();
		started = 1;
	}

	if(current_cred()->uid.val == proc){ //If the pid is unblocking itself
		mutex_lock(&lockTable); //Lock
		sysTable *check;
		sysTable *procHash = getTableMain(nr); //Get the hashtable from array 
		struct hlist_node *temp;

		hash_for_each_possible_safe(procHash, check, temp, h_hash_list, proc){ //Check for pid
			if(check->proc == proc && check->call == nr){ //If the pid and nr is found
				printk("Process %d can now use %ld\n", proc, nr);

				hash_del(&check->h_hash_list); //Remove the block
				kfree(check); //Free memeory
				mutex_unlock(&lockTable);
				return 0;
			}
		}	

		mutex_unlock(&lockTable); //Unlock
		printk("Error: Process %d is not blocked from using %ld\n", proc, nr);
	}
	else if(current_cred()->uid.val == 0){ //If pid is root
		mutex_lock(&lockTable); //Lock
		sysTable *check;
		sysTable *procHash = getTableMain(nr); //Get the hashtable from array

		struct hlist_node *temp;
		hash_for_each_possible_safe(procHash, check, temp, h_hash_list, proc){ //Check for pid
			if(check->proc == proc && check->call == nr){ //If the pid and nr is found
				printk("Process %d can now use %ld\n", proc, nr);

				hash_del(&check->h_hash_list); //Remove the block
				kfree(check); //Free memeory
				mutex_unlock(&lockTable);
				return 0;
			}
		}

		mutex_unlock(&lockTable); //Unlock
		printk("Error: Process %d is not blocked from using %ld\n", proc, nr);
	}
	else{ //If the pid is trying to block another pid but is not root
		printk("Error: Process needs to be root to block other PID's calls\n");
		return -EPERM;
	}
	return -ENOENT //Return error if not found
}

/*
*Return number of times a the process proc called the systemcall nr
*/
SYSCALL_DEFINE2(sbx421_count, pid_t, proc, unsigned long, nr){
	if(started == 0){ //Check if initlized
		initalizeHash();
		started = 1;
	}

	if(current_cred()->uid.val == 0){ //Check for root and valid proc
		if(proc != 0){
			mutex_lock(&lockTable); //Lock
			sysTable *check;
			sysTable *procHash = getTableMain(nr); //Get the hashtable from array 
			
			hash_for_each_possible(procHash, check, h_hash_list, proc){ //Look though hash table
				if(check->porc == proc && check->call == nr){ //If pid and nr is found
					printk("Process %d called systemcall %ld: %d times\n", proc, nr, check->count);
					mutex_unlock(&lockTable);
					return check->count;
				}
			}

			//Call count
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
	if(started == 0){ //Check if initlized
		initalizeHash();
		started = 1;
	}

	sysTable *check;
	sysTable *procHash = getTableMain(nr); //Get the hashtable from array
	//getHash(uid);
	
	hash_for_each_possible(procHash, check, h_hash_list, proc){
		if(nr == procHash->call){ //Check if the syscall is in the table
			printk("Error: UID %d is blocked from using %ld\n", uid, nr);
			return 1;
		}
	}
	
	printk("UID %d is not blocked from using %ld\n", uid, nr);
	return 0;
}

/*
* Return the hashtable for syscall
*/
sysTable getTableMain(unsigned long index){
	return *permList[index];
}

/*
* Initalize the array with empty hashtables
*/
void initalizeHash(void){
	int countm = 0;

	for(countm = 0; countm <= 436; countm++){ //Intalize hashtables
		DEFINE_HASHTABLE(hashed, 16); 
		permList[countm]->h_hash_list = hashed;
		//DEFINE_HASHTABLE(hashed, 16); //Declare hashtable
		//permList[countm] = hash_init(hashed, 16);//Add hashtable to the array
	}
}

//int getHash(int key){
//	return key % SIZE;	
//}

