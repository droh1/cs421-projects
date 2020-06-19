/*
* hash.h (holds basic hash table functions)
* Testing a manual hashtable impmentaion
*/

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/errno.h>
#include <linux/cred.h>
#include <linux/mutex.h>
//#include <asm/uaccess.h>
//#include <linux/slab.h>

#define SIZE 30 //Change if collisions occur

struct table{
	int data;
	int key;
};

struct table* hashTable[SIZE];
struct table* temp;
struct table* item;

/*
* Calculate hash
*/
int getHash(int key){
	return key % SIZE;
}

/*
* Find a item in hash table
*/
struct table *find(int key){
	int index = getHash(key); //Find hashed index

	while(hashTable[index] != NULL){
		if(hashTable[index]-> key == key){ //If data is found
			return hashTable[index];
		}
		
		index = index + 1; 
		index = index % SIZE;
	}

	printk("Unable to find key of %d", key); 
	return NULL;
}

/*
* Insert item to hash table
*/
int insert(int key, int data){
	struct Table *temp = (struct Table*)kmalloc(sizeof(struct table));
	
	if(temp == NULL){ //Check allocation
		printk("Error: Unable to allocate memory\n");
		return 1; //Return 1 for error
	}

	temp->data = data;
	temp->key = key;
	
	int index = getHash(key); //Find Hashed index

	while(table[index] != NULL){ //Look for empty location
		index = index + 1;
		index = index % SIZE;
	}

	table[index] = item;
	return 0; //Return 0 for sucess
}

/*
* Remove item from hash table
*/
struct table* remove(struct table* item){
	int key = item->key;
	int index = getHash(key);
	
	while(table[index] != NULL){
		if(table[index]->key = key){ //If the key is found and not empty
			struct table* del = table[index];
			table[index] = temp;
			return del;
		}
	
		index = index + 1;
		index = index % SIZE;
	}
	
	return NULL;
}

