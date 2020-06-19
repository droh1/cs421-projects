//Testing of system call 4
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define __NR_sbx421_block 434
#define __NR_sbx421_unblock 435
#define __NR_sbx421_count 436


long block_syscall(int uid, unsigned long nr){
	return syscall(__NR_sbx421_block, uid, nr);
}

long unblock_syscall(int uid, unsigned long nr){
	return syscall(__NR_sbx421_unblock, uid, nr);
}

long count_syscall(int uid, unsigned long nr){
	return syscall(__NR_sbx421_count, uid, nr);
}

int main(int argc, char *argv[]){
		long rv, rr;
	long rv;
	//int test1;
	//long test2;
	int x = 1;
	
	printf("NOTE: not fully implmented yet\n");
	if(argc < 3){
		printf("Error, please enter two values as arguments\n");
		return 1;
	}
	
	//Note test 1 is a username and test 2 is a file
	//test1 = strtol(argv[1], NULL, 10); 
	//test2 = strtol(argv[2], NULL, 10); 	

	/*if(rr == 2){
		perror("Count passed\m");
	}
	else{
		printf("Count failed\n");
	}*/

	return 0;
}
