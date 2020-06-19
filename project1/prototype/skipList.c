/*
* Skiplist Prototype
*
*
*/
#include "skipList.h"

/*
* Calculate a random int from 0 to 32767
*/
static unsigned int get_random_int(void){ //This code is taken from project website
	next_random = next_random * 1103515245 + 12345;
	return ((next_random / 65536) % 32768);
}

/*
* Gain a new seed
*/
static void seed_random(unsigned int seed){ //This code is taken from project website
	next_random = seed;
}

/*
* Create MailBox
*/
mailBox *createMailBox(char *msg){
	mailBox *temp = malloc(sizeof(mailBox)); //Allocate memory for mailbox

	temp->msg = msg; //Set the message
	temp->nextMail = NULL; //Set next pointer in list
	return temp;
}

/*
* Create a skiplist
*/
slist *createSkipList(int maxHeight, int chance){
	slist *temp = malloc(sizeof(slist)); //Allocate memeory for list

	temp->maxHeight = maxHeight; 
	temp->chance = chance; //Set the chance of creating a new node
	temp->start = createNode(1, -1, maxHeight, 0); //Default node settings
	
	return temp;
}

/*
* Add a node to the skiplist
*/
slist *createNode(int start, int key, int height, int uid){
	node * temp = malloc(sizeof(node)); //Allocate memeory for node
	temp->next = malloc(height * sizeof(node)); //Allocate memeory for next node
	
	int a; 
	for(a = 0; a < height; a++){
		temp->next[a] = NULL; //Set pointers to NULL
	}
	
	//Set struc variables
	temp->key = key;
	temp->height = height;
	temp->start = start;
	temp->uid = uid;

	return temp;
}

/*
* Delete Skiplist
*/
void deleteSkip(slist *list){
	node *temp; //Iterators
	node *next; 

	temp = list->start; //Set the first iterator to the beginning of the list
	int a;
	
	while(temp != NULL){ //Go though the skiplist until the end is reached
		next = temp->next[0]; //Get next item in the list
		free(temp->next); //Delete the allocated memory in the current list
		temp->next = NULL; //Remove the pointer
		free(temp); //Delete the current node
		temp = next; //Go to next item in list
	}
	
	free(list); //Delete memory for list
	list = NULL; //Remove pointer
}

/*
* Search skiplist
*/
node *skipSearch(slist *list, int key){
	node *temp = list->start; //Start at beginning of list
	int height = ((list->maxHeight) - 1); //Start at the top
	
	while(temp != NULL){
		if(temp->key == key){ //Check the key is at the current node
			return temp;
		}else if(temp->key > key){ //Check if the key is smaller
			if(height == 1){ //If the buttom of the list is reached
				return NULL;
			}else{ //Move to the next level
				height--;
			}
		}else if(temp->key < key){ //Check if the key is larger
			if(temp->next[0] == NULL){ //If the key does not exist
				return NULL;
			}else{ //Go to the next node
				temp = temp->next[0];
			}
		}
	}
	return NULL;  //If the key dosent exist
}

/*
* Chance of getting next level
*/
int upHeight(slist *list){
	int count = 1;
	int temp = list->chance; //Get the chance set for this list 
	int chance, check;

	if(temp == 2){ //Check the chance set for the list (uses base 2 - 1)
		check = 16383;
	}else if(temp == 4){
		check = 8191;
	}else if(temp == 8){
		check = 4095;
	}else if(temp == 16){
		check = 2047;
	}
	
	if(get_random_int() > check){ //If smaller then a new node is not made and starts at current list
		chance = 0; 
	}else{//If larger a new node is made a level above
		chance = 1; 
	}

	while(count && chance <= list->maxHeight){ //If the hight is smaller then the set maxHeight
		if(count < list->maxHeight){ //Check if the hight is smaller then maxHeight
			count++;
		}
		chance++;
	}
	
	return count; //Return the remaining before hitting the maxHeight
}

/*
* Insert a key to the skiplist
*/
void insertKey(slist *list, int key, int uid){
	int height; //Height of previous
	int curr; //Height of current
	int a; //counter	

	slist *temp = list->start; //Start of list
	slist **last; 
	slist *new;

	last = malloc(list->maxHeight * sizeof(node)); //Allocate for memeory for new node

	for(a = 0; a < list->maxHeight; a++){
		last[a] = list->start; //Set pointers to first position on list
	}

	curr = ((list->start) - 1); //Starting height of new list

	while(temp != NULL){
		if(temp->start->next[curr] == NULL){ //If the end of the list is reached 
			if(temp > 0){ //If height not at the bottom
				last[curr] = temp; 
				curr--;
			}
			else{ //If at the bottom of the list
				last[curr] = temp; //
				height = upHeight(list); //Get the height above
				new = createNode(0, key, height, uid); 

				for(a = 0; a < height; a++){ //Connect the new node
					if(last[a]->start->next[0] == NULL){ //If the node is being inserted to the end
						new->start->next[a] = NULL;
					}else{ //If the node is being added between
						new->start->next[a] = last[a]->start->next[a];
					}
				}

				for(a = 0; a < list->maxHeight; a++){ //Connect old node to new one
					last[a]->start->next[a] = new;
				}

				temp = NULL; //Break while loop
			}
		}
		else if(key < temp->start->next[curr]->key){ //If inserting key is smaller then the current key
			if(curr > 0){ //If height is not at bottom
				last[curr] = temp;
				curr--;
			}else{ //If at bottom
				last[curr] = temp;
				height = upHeight(list); //Get height above
				temp = createNode(0, key, height, uid);

				for(a = 0; a < height; a++){ //Connect new node
					if(last[a]->start->next[0] == NULL){ //If the node is being inserted to the end
						temp->start->next[a] = NULL;
					}else{ //If the node is being added between
						temp->start->next[a] = last[a]->start->next[a];
					}				
				}
				
				for(a = 0; a < list->maxHeight; a++){ //Connect old node to new one
					last[a]->start->next[a] = temp;
				}

				temp = NULL; //Break while loop
			}
		}else if(key > temp->start->next[curr]->key){ //If the key is bigger then current key
			temp = temp->start->next[curr]; //Go to the next node
			last[curr] = temp;
		}
	}

	free(last); 
}

/*
* Delete a key from the skiplist
*/
void deleteKey(slist *list, int key, int uid){
	//NOTE TO SELF, May have mixed up the pointers for deleting the key. Fix later
	printf("deleteKey is not implmented\n"); //REMINDER
}
/*	int height; //Height of previous
	int curr; //Height of current
	int a; //counter	

	slist *temp = list->start; //Start of list
	slist **last; 
	slist **new;

	last = malloc(list->maxHeight * sizeof(node)); //Allocate for memeory for new node

	for(a = 0; a < list->maxHeight; a++){
		last[a] = list->start; //Set pointers to first position on list
	}

	curr = ((list->start) - 1); //Starting height of new list

	while(temp != NULL){
		if(temp->start->next[curr] == NULL){ //If the end of the list is reached
			if(temp > 0){ //If height not at the bottom
				last[curr] = temp; 
				curr--;
			}
			else{ //If at the bottom of the list
				last[curr] = temp; 

				if(last[a]->start->next[0] == NULL){ //If current node is the end
					new[a]->start = last[a]->start; //Set new for node to delete
					free(new[a]->start); //Delete first node
					last[a]->start = NULL; 
				}else{ //If the there is another node
					new[a]->start = last[a]->start->next[0]; //Set new for node to delete
					free(last[a]->start); //Delete node
					last[a]->start = new[a]->start; //Connect the next node to start
				}

				temp = NULL; //Break while loop
			}
		}
		else if(key < temp->start->next[curr]->key){ //If inserting key is smaller then the current key
			if(curr > 0){ //If height is not at bottom
				last[curr] = temp;
				curr--;
			}else{ //If at bottom
				last[curr] = temp;

				for(a = 0; a < height; a++){ //Connect new node
					if(last[a]->start->next[0] == NULL){ //If the curent node is the end
												
						temp->start->next[a] = NULL;
					}else{ //If the node is being added between
						temp->start->next[a] = last[a]->start->next[a];
					}				
				}
				
				for(a = 0; a < list->maxHeight; a++){ //Connect old node to new one
					last[a]->start->next[a] = temp;
				}

				temp = NULL; //Break while loop
			}
		}else if(key > temp->start->next[curr]->key){ //If the key is bigger then current key
			temp = temp->start->next[curr]; //Go to the next node
			last[curr] = temp;
		}
	}

	free(last); 
}*/

/*******************************
* Main function here
*******************************/
int main(void){
	/*printf("Running Test\n");
	slist test;

	test = createSkipList();
	
	if(test == NULL){
		printf("ERROR: skiplist not made\n");
		exit(1);
	}
*/
	

	return 0;
}

