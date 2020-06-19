README.txt

For this HW, I first started by thinking that this hw required us to emulate the shell by using our own 
defined functions regarding the shell. I later found out that exec() took care of commands and will end
up being how we emulate the shell. After finding this out, I started by figuring out how to read in user
input. The challenge was that we are unsure of how many commands will be read in at any given moment. This
lead me to use dynamic memory as then I can account for small commands as well as crazy large ones. Once
The memory was figured out, I needed a way to parse though the input so that exec() can use it. I found that
the shell comands are mostly seprated by a space in which the first line before the space is the command and 
the commands after the space are normally the arguments. I was able to use strtok() to seprate the input by
the spaces and place them into array. After that it was up up to sending the array to exec() so it can be 
executed. 


QUESTIONS:

1. A software system call is a interrupt that is made by a program.
	-When a program has "exit(1)"
   A interrupt is a hardware call which tells the hardware that somthing new is ready for processing.
	-When a keyboard is being typed on
   A trap is a an exception which is made once an instruction is run.
	-"1/0"

2. Fork() makes a copy of the current running process so that it can run somthing else
   exec() is a command that takes an argument of special keywords to execute a process

   When used together, they can execute another process that may need to be processed first without
   causing major issues in the current process.

3. Monolithic Kernel and Micro Kernel simular as they both can be used to execute processes.

   Monolithic Kernel is diffrent from a micro kernel beacuse monlithic kernels only run processes on the 
   kernel space. Also, monolithic kernels are hard to extend due to the non modular approch.

   Micro Kernel is diffrent from a monlithic because microkernals have seprate processes which can run in 
   the kernel or in the user space. Also, micro kernels are easier to extend due to the modular approch.