/*
* Test file to test project1.c
*/


int main(void){
	printf("Running test: \n");

	char msg1[] = "This is a test";
	char msg2[] = "This is also a test";

	//TEST 1
	long test1 = mbx421_init(5, 10); //Initalise skiplist

	if(test1 == 0){
		printf("test 1 passed\n");
	}else{
		printf("test 1 failed: Returned %ld\n", test1);
	}

	sleep(5);

	//TEST 2
	long test2 = mbx421_create(142);
	
	if(test2 == 0){
		printf("test 2 passed\n");
	}else{
		printf("test 2 failed: Returned %ld\n", test1);
	}
	
	sleep(5);

	//TEST 3
	long test3 = mbx421_send(142, msg1, 14);
	
	if(test3 == 0){
		printf("test 3 passed\n");
	}else{
		printf("test 3 failed: Returned %ld\n", test1);
	}

	sleep(5);

	//TEST 4
	long test4 = mbx421_rev(142, msg1, 14);
	
	if(test4 == 0){
		printf("test 4 passed\n");
	}else{
		printf("test 4 failed: Returned %ld\n", test1);
	}
	
	sleep(5);

	//Test 5 (test for wrong user removal
	long test5 = mbx421_rev(4, msg1, 14);
	
	if(test5 != 0){
		printf("test 5 passed\n");
	}else{
		printf("test 5 failed: Returned %ld\n", test1);
	}

	sleep(5);

	//TEST 6
	long test6 = mbx421_rev(142, msg1, 14);
	
	if(test5 == 0){
		printf("test 6 passed\n");
	}else{
		printf("test 6 failed: Returned %ld\n", test1);
	}

	sleep(5);

	//TEST 7
	long test7 = mbx421_shutdown();
	
	if(test7 == 0){
		printf("test 7 passed\n");
	}else{
		printf("test 7 failed: Returned %ld\n", test1);
	}

	printf("Closing Test\n");
}
