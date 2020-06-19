/*
* project1.c
* Kernel Commands
*/
#include <linux/cred.h> //For getting uid
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/syscalls.h>
#include <linux/time.h>

//Prototypes
static unsigned int get_random_int(void);
static void seed_random(unsigned int seed);

//Struct for skiplist
typedef struct slist{
	int height;
	int size;
	struct nodeS *header;
}slist;

//Nodes in skiplist
typedef struct nodeS{
	unsigned long box_id; //Key
	struct mailbox *box;
	struct nodeS **next;
}nodeS;

//Struct for mailboxes
typedef struct mailBox{
	bool aclC;
	unsigned long id;
	unsigned long count;

	struct list_head list_node;
	struct list_head messages;
	struct list_head acl;
}mailbox;

//Struct for messages in mailbox
typedef struct message{
	unsigned char *msg;
	unsigned long length;

	struct list_head list_node;
}message;

//Struct for acl
typedef struct acl{
	pid_t pid;
	
	struct list_head list_node;
}acl_list;

//Global Variables
LIST_HEAD(mail); 
slist skiplist;
int MAILBOXC = 0;
int maxHeight;
int chance;
static unsigned int next_random = 9001;
unsigned long MAXID = 4611690000000000000;

/****************************START OF KERNEL CODE**************************************
***************************************************************************************/

SYSCALL_DEFINE2 (mbx421_init, unsigned int, ptrs, unsigned int, prob){
	if(MAILBOXC != 0){ //If the mailbox was already made
		printk("Error: Mailbox already exists");
		return -EEXIST;
	}

	if(current_uid().val != 0){ //Check if user is root
		printk("Error: User is not root\n");
		return -EPERM; //Return error for bad permissions
	}
	
	if(ptrs <= 0){ //If no ptrs is given
		printk("Error: No ptrs passed in\n");
		return -EINVAL; //Return error for invalid value
	}
	
	if(prob != 2 || prob != 4 || prob != 8 || prob != 16){ //Check if proper prob is passed
		printk("Error: Prob can only be 2, 4, 8, or 16\n");
		return -EINVAL; //Return error for invalid value
	}

	printk("Initialising mailbox\n");

	MAILBOXC = MAILBOXC + 1;
	
	printk("Mailbox Created\n");
	return 0; //If successful
	
/* //Skiplist implmentaion
	int value = get_random_int();
	
	struct slist *temp = kmalloc(sizeof(*slist)); //Testing this
	if (temp == NULL){
		printk("Error: Failed to allocate memeory\n");
		return -ENOMEM;
	}

	skiplist = kmalloc(sizeof(slist), GFP_KERNEL); //Allocate memeory for list
	
	if(skiplist == NULL){ //Check allocation
		printk("ERROR: Skiplist failed to get memory\n");
		return -ENOMEM;
	}

	maxHeight = ptrs; 
	chance = prob; //Set the chance of creating a new node
	
	//Create first node 
	node * temp = (node*)kmalloc(sizeof(node), GFP_KERNEL); //Allocate memeory for node
	
	if(temp == NULL){ //Check memeory allocation
		printk("ERROR: Node failed to get memory\n");
		return -ENOMEM;
	}

	skiplist->header = temp;
	temp->box_id = 0;

	temp->next = (node **)kmalloc((skiplist->maxHeight + 1 * sizeof(node)), GFP_KERNEL); //Allocate memeory for next node
	
	if(temp->next == NULL){
		printf("ERROR: Node->next failde to get memeory\n");
		return -ENOMEM;
	}

	int a; 
	for(a = 0; a < skiplist->maxHeight; a++){
		temp->next[a] = skiplist->header;
	}
	
	//Set skiplist variables
	skiplist->height = 1;
	skiplist->size = 0;
*/
}

SYSCALL_DEFINE0 (mbx421_shutdown){
	printk("Shutting down mailbox\n");

	if(current_uid().val != 0){ //Check if user is root
		printk("Error: User is not root\n");
		return -EPERM; //Return error for bad permissions
	}
	
	if(MAILBOXC == 0){
		printk("Error: Mailbox does not exist");
	}
	
	struct list_head *temp;
	mailbox *box = NULL;

	list_for_each(temp, &mail){
		box = list_entry(temp, mailbox, list_node);
		
		if(box != NULL){
			list_del(&box->messages);
			list_del(&box->acl);
			list_del(&box->list_node);
		}
	}

	list_del(&mail);
	/*
	nodeS *temp; //Next node
	nodeS *rem; //Node to remove
	rem = mail->header;
	rem = rem->next;

	while(rem != NULL){ //Check if the struct node has data
		temp = rem->next[0];
		kfree(rem->next);
		rem->next = NULL;
		kfree(rem);
		rem = temp;
	}

	rem = mail->header; //Go back to header node
	temp = rem->box; //Go into mailbox box
	rem->next = NULL;
	rem = temp;	

	if(rem != NULL){ //Check if the struct mailbox has data
		if(rem->messages != NULL){ //Check if struct messages has data
			temp = rem->messages;
			list_del(temp->list_node);
		}
		if(rem->list_node != NULL){ //Check if struct list_node has data
			temp = rem->messages;
			list_del(temp->list_node);
		}
		if(rem->acl != NULL){ //Check if struct acl has data
			temp = rem->acl;
			if(temp->list_node != NULL){
				temp = temp->list_node
				list_del(temp->list_node);
				temp = rem->acl;			
			}
		}
		
		//free data in mailbox
		list_del(rem->messages);
		list_del(rem->acl);
		list_del(rem->list_node);
		kfree(rem);
	};
*/
	MAILBOXC = 0;

	return 0; //If Succesful
}

SYSCALL_DEFINE1 (mbx421_create, unsigned long, id){
	if(MAILBOXC == 0){ //If no mailbox exists
		printk("Error: Mailbox has not started");
		return -ENOENT;
	}
	if(id == 0 || id < MAXID){ //Check if the id is valid
		printk("Error: Invalid ID");
		return -EFAULT;
	}

	if(current_uid().val != 0){ //Check if user is root
		printk("Error: User is not root\n");
		return -EPERM; //Return error for bad permissions
	}
	

	printk("Creating mailbox for %lu\n", id);

	struct list_head *temp;
	mailbox *box = NULL;

	list_for_each(box, &temp){ //Check if mailbox exists
		box = list_entry(temp, mailbox, list_node);
		if(box->id == id){
			printk("Error: Mailbox already exists\n");
			return -EEXIST;
		}
	}

	mailbox *boxN;
	boxN = (mailbox*)kmalloc(sizeof(mailbox), GFP_KERNEL);
	
	if(box == NULL){ //Check allocation
		printk("Error: Unalbe to allocate memory for mailbox\n");
		return -ENOMEM;
	}

	//Set variables
	boxN->id = id;
	boxN->count = 0;
	list_add_tail(&boxN->list_node, &mail);
	
	//Initalize the sub lists
	INIT_LIST_HEAD(&boxN->messages);
	INIT_LIST_HEAD(&boxN->acl);

/*
	//Code for SkipList Implmentaion
	nodeS *update[maxHeight + 1];
	nodeS *temp = skiplist->header;
	int i, h;

	for(i = skiplist->height; i >= 1; i--){
		while(temp->next[i]->id < id){
			temp->temp->next[i];
		}

		update[i] = temp;
	}
	temp = temp->next[1];

	if(id == temp->id){
		temp->id = id;
		return 0;
	}else{
		h = get_random_int();
		if(h > skiplist->height){
			for(i = skiplist->height + 1; i < h; i++){
				update[i] = skiplist->header;
			}
		}

		skiplist->height = height;
	}

	temp = (nodeS*) kmalloc(sizeof(nodeS), GFP_KERNEL);
	temp-> id = id;
	temp->next = (nodeS**) kmalloc(sizeof(nodeS) * (height + 1), GFP_KERNEL);
	
	for(i = 1; i <= height; i++){
		temp->next[i] = update[i]->next[i];
		update[i]->next[i] = temp;
	}
*/

	return 0; 
}

SYSCALL_DEFINE1 (mbx421_destroy, unsigned long, id){
	if(MAILBOXC == 0){ //If no mailbox exists
		printk("Error: Mailbox has not started");
		return -ENOENT;
	}

	if(current_uid().val != 0){ //Check if user is root
		printk("Error: User is not root\n");
		return -EPERM; //Return error for bad permissions
	}

	printk("Deleting mailbox %lu\n", id);
	
	struct list_head *loc;
	struct list_head *temp;
	mailbox *box = NULL

	list_for_each_safe(loc, temp, &mail){ //Look tho the list
		box = list_entry(loc, box, list_node);
		
		if(box->id ==id){ //If the mailbox was found
			if(box->count > 0){ //If the mailbox not empty
				printk("Error: Mailbox still has messages\n");
				return -EINVAL;
			}
		list_del(loc);
		kfree(box);
		printk("Mailbox %lu deleted\n", id);

		return 0;
		}
	}
/*	//Skiplist Impmentation
	nodeS *update[maxHeight + 1];
	nodeS *temp = slist->header;

	int i;
	for(i = slist->height; i >= 1; i--){ 
		while(temp->next[i] < id){ //Go right until value is larger
			temp = temp->next[i];
		}
		update[i] = temp;
	}

	temp = temp->next[1];

	if(temp->box_id == id){ //If the id is found
		for(i = 1; i <= slist->height; i++){
			if(update[i]->next[i] != temp){
				break;
			}
			update[i]->next[i] = temp->next[i];
		}
		
		if(temp){ //Delete the node
			kfree(temp->next);
			kfree(temp);
		}

		while(slist->height > 1 && slist->header->next[slist->height] == slist->header){ //Update skiplist
			slist->height = slist->height - 1;
		}

		return 0;
	}
*/
	printk("Error: ID %lu was not found in mailbox\n", id); 
	return -ENOENT; //End of skiplist was reached and id was not found
}

SYSCALL_DEFINE1 (mbx421_count, unsigned long, id){
	struct list_head *temp;
	mailbox *box = NULL;
	
	list_for_each(temp, &mail){
		box = list_entry(temp, box, list_node);
		if(box->id == id){			
			if(current_uid().val != id){ //Check if user has does not have permission
				printk("You do not have permissions to read mailbox: %lu but it has %lu\n", box->id, box->count);
			}else{
				printk("You can read mailbox: %lu which has %lu\n", box->id, box->count);
			}

			return 0; //Success
		}
	}
	
	printk("Error: Mailbox %lu was not found\n", id);
	return -EEXIST;
}

SYSCALL_DEFINE3 (mbx421_send, unsigned long, id, const unsigned char __user*, msg, long, len){
	struct list_head *temp;
	mailbox *box = NULL;
	message *msgS = NULL;

	printk("Preparing to send message\n");

	if(MAILBOXC == 0){ //If no mailboxs exists
		printk("Error: Mailbox %lu does not exist", id);
		return -ENOENT;
	}
	
	if(id == 0 || id < MAXID){ //Check if the id is valid
		printk("Error: Invalid ID");
		return -EFAULT;
	}

	if(current_uid().val != id){ //Check if user has access to mailbox
		printk("Error: You dont not have access to this mailbox\n");
		return -EPERM; //Return error for bad permissions
	}

	if(len < 0){
		printk("Error: Message length is invalid\n");
		return -EINVAL;
	}
	
	list_for_each(temp, mail){
		box = list_entry(temp, mailbox, list_node);
		
		if(box->id == id){ //If the mailbox is found
			box->count = box->count + 1; //Update the number of messages in mailbox
			msgS = kmalloc(sizeof(msgS), GFP_KERNEL); //Allocate memory for struct
			
			if(msgS == NULL){ //Check if memory was allocated properly
				printk("Error: Failed to allocate memeory\n");
				return -ENOMEM;
			}

			msgS->msg = kmalloc(len, GFP_KERNEL); //Allocate memeory for message
			
			if(msgS->msg == NULL){ //Check if memeory was allocated poperly
				printk("Error: Failed to allocate memeory\n");
				return -ENOMEM;
			}

			copy_from_user(msgS->msg, msg, len); //Copy message from user space

			msgS->length = len;
			list_add_tail(&msgS->list_node, &box->messages); //Add message to mailbox
			
			printk("Message sent to %lu\n", id);

			return 0;
		}
	}
	//If the mailbox id is not found
	printk("Error: Mailbox %lu does not exist\n", id);
	return -ENOENT;
}

SYSCALL_DEFINE3 (mbx421_recv, unsigned long, id, unsigned char __user*, msg, long, len){
	unsigned long length;
	struct list_head *temp;
	mailbox *box = NULL;
	message *msgR = NULL;

	printk("Checking received messages\n");

	if(MAILBOXC == 0){ //If no mailboxs exists
		printk("Error: Mailbox %lu does not exist", id);
		return -ENOENT;
	}
	
	if(id == 0 || id < MAXID){ //Check if the id is valid
		printk("Error: Invalid ID");
		return -EFAULT;
	}

	if(current_uid().val != id){ //Check if user has access to mailbox
		printk("Error: You dont not have access to this mailbox\n");
		return -EPERM; //Return error for bad permissions
	}

	list_for_each(temp, mail){
		box = list_entry(temp, mailbox, list_node);
		
		if(box->id == id){ //If mailbox is found
			if(box->count == 0){ //If no messages are in mailbox
				printk("Error: Mailbox has no messages\n");
				return -ESRCH;
			}
			
			msgR = list_first_entry(&box->messages, message, list_node);
			printk("Reveived message: %s\n", msgR->msg);
			length = msgR->length;

			list_del(&msgR->list_node); //Delete message
			kfree(msgR);
			
			return length; //Return length of message
		}
	}

	//If the mailbox id is not found
	printk("Error: Mailbox %lu does not exist\n", id);
	return -ENOENT;
}

SYSCALL_DEFINE1 (mbx421_length, unsigned long, id){
	struct list_head *temp;
	mailbox *box = NULL;
	message *tempM = NULL;

	list_for_each(temp, mail){ 
		box = list_entry(temp, box, list_node);

		if(box->count == 0){ //Check if the mailbox has mail
			printk("Error: No messages found in mailbox %lu\n", box->id); 
			return -EINVAL;
		}

		tempM = list_first_entry_or_null(&box->messages, msg, list_node);

		if(tempM == NULL){ //Check if there is a message but is NULL
			printk("Error: No messages found in mailbox %lu\n", box->id);
			return -EINVAL;
		}
		
		int size = (sizeof(tempM->msg) / sizeof(*tempM->msg)); //Get the size of message

		if(current_uid().val != id){ //Check if user has does not have permission
			printk("You do not have permissions to read mailbox: %lu but it has message of size %u\n", box->id, size);
		}else{
			printk("You can read mailbox: %lu which has a message of size %u\n", box->id, size);
		}
	}

	return 0; //Success
}

SYSCALL_DEFINE2 (mbx421_acl_add, unsigned long, id, pid_t, proccess_id){
	struct list_head *temp;
	struct list_head *acl;
	mailbox *box = NULL;
	acl_list *cacl = NULL;

	printk("Adding to ACL\n");

	if(current_uid().val != 0){ //Check if user is root
		printk("Error: User is not root\n");
		return -EPERM; //Return error for bad permissions
	}

	list_for_each(temp, mail){ //Look though the mailboxes
		box = list_entry(temp, mailbox, list_node);
		
		if(box->id == id){ //If mailbox is found
			list_for_each_entry(acl, acl_list, list_node){ //Look in acl
				cacl = list_entry(acl, acl_list, list_node);

				if(cacl->pid == proccess_id){
					printk("Error: PID %u is already in mailbox %lu\n", proccess_id, box->id);
					return -EINVAL;
				}
			}

			box->aclC = true;
			cacl = kmalloc(sizeof(acl_list), GFP_KERNEL); //Allocate memory for acl
			
			if(cacl == NULL){ //Check if memeory allocated properly
				printk("Error: Failed to allocate memeory for acl\n");
				return -ENOMEM;
			}

			cacl->pid = proccess_id;
			list_add_tail(&cacl->list_node, &box->acl);

			return 0;
		}
	}
	
	printk("Error: Unable to find mailbox with id %lu\n", id);
	return -EINVAL;
}

SYSCALL_DEFINE2 (mbx421_acl_remove, unsigned long, id, pid_t, process_id){
	struct list_head *temp;
	struct list_head *acl;
	mailbox *box = NULL;
	acl_list *cacl = NULL;

	printk("Removing from ACL\n");

	if(current_uid().val != 0){ //Check if user is root
		printk("Error: User is not root\n");
		return -EPERM; //Return error for bad permissions
	}
	
	list_for_each(temp, &mail){ //Look though the mailboxes
		box = list_entry(temp, mailbox, list_node);

		if(box->id == id){ //If mailbox is found
			list_for_each(acl, &box->acl){ //Look though acl
				cacl = list_entry(acl, acl_list, list_node);
				
				if(cacl->pid == process_id){ //If process id is found
					printk("Removed %u from mailbox\n", cacl->pid);
					list_del(&cacl->list_node);
					kfree(cacl);

					return 0; //Succesful
				}
			}

			//Unable to find PID
			printk("Error: PID %u is was not found\n", process_id);
			return -EINVAL;
		}
	}

	//Unable to find mailbox
	printk("Error: Mailbox was not found\n");
	return -EINVAL;
}


/********************Start of helper functions*****************************
***************************************************************************/

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











