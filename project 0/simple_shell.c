/*
* File: simple_shell.c
* Class: CSMC 421
* Author: Daniel Roh
*
* Create a simple shell using exec() and fork()
*
* NOTE TO SELF: DO NOT DEV ON DEBIAN
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/wait.h>
 
void process(char *input, char **command){
    const char *txt = " ";
    int tokens = 0;

    command[tokens] = strtok(input, txt); //Get the first command
    tokens++;

    while(command[tokens-1] != NULL){
        command[tokens] = strtok(NULL, txt);
        tokens++;
    }
	//printf("TOKENS: %d\n", tokens);//DEBUG
}
 
void run(char **command){
    pid_t pid;
    int status;

    if((pid = fork()) < 0){
        printf("Error: Failed to fork, please get new fork\n");
        exit(1);
    }else if(pid == 0) {
        if(execvp(command[0], command) < 0){
            //printf( "Error: Exec failed\n"); //DEBUG
            exit(1); //End forked process
        }
    }else{
        while(wait(&status) != pid){
            //Child is running, wait till finished
        }
    }
}
 
int main(void){ //int argc, char *argv[]
    bool test = true;
    char *input;
    char **command;
       
    while(test == true){
        int buffer = 1024; //Default buffer size
        
        printf("$:");
		command = malloc(sizeof(char*) *buffer); //Allocate memory
		input = malloc(sizeof(char*) * buffer);//

		int tester = 0;
		while(tester < buffer){ //Initalise values
			command[tester] = '\0';
			input[tester] = '\0';
			tester++;
		}
		
		if(input == NULL){ //Check if malloc worked
			printf("Error: Unalbe to allocate memory\n");
			exit(1);
		
		}
		
        int temp = EOF;
        int count = 0;
		
        while((temp = getchar()) != '\n' && temp != EOF){
        	input[count++] = (char)temp;

			if(count == buffer){ //If the max buffer was reached
				char *tempBuffer;
				char **tempArr;
				buffer = buffer * 2; //Double Buffer
		   
                tempBuffer = realloc(input, (sizeof(char*) * buffer));
                tempArr = realloc(command, (sizeof(char*) * buffer));
		
				int tester2 = (buffer / 2);
				while(tester2 < buffer){ //Initalise values
					tempArr[tester2] = '\0';
					input[tester] = '\0';
					tester2++;
				}

				if(tempBuffer == NULL || tempArr == NULL){ //If realloc failed
					printf("Error: Unable to make bigger buffer\n");
					free(input);
					free(command);
					exit(0);
				}
				
				input = tempBuffer;
				command = tempArr;
			}
             
			input[count] = '\0'; //Add null at the end
			
			if(strcmp(input, "exit") == 0){ //If exit is called
				free(input);
				free(command);
				exit(0);
			}
		}
		
		process(input, command);
         
		if(command == NULL){
			//printf("DEBUG: NO COMMANDS ENTERED\n");//DEBUG
		}else{
			run(command); 
		}
		
		//Free Memeory
		free(input);
		free(command);
		
    }

    return 0;
}
