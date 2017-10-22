# Dining-Philosophers
The code allows a user to simulate the dining philosopher problem.
There is N philosophers around a table each with a fork to their left and right. A philosopher needs two forks to eat their spaghetti. The code needed to prevent deadlocks, a situation where philosophers all grab forks to their left and wait for the fork on their right to be available. To avoid this, a mutex was used to create an area for one philosopher to eat. Said philosopher grabs a fork to the left and right side of him/her. If a philosopher cannot grab both forks, then they wait until they can. The program is in an infinite loop and runs forever. To exit the program the user needs to hit ctr+c. After this signal is received by the program, all of the semaphores are closed and the program is exitted. This code also implements pthread for each of the philosophers.

To run the code, I've included a Makefile.
From the terminal, type >> make all
This will compile dphil.c
The program is then run from the terminal with the following arguments:
>>./dphil <number of philosophers> <number of knives> <time delay>
  Example: <number of philosophers> = 5, <number of knives> = 3, <time delay> = 1
           >>./dphil 5 3 1
  
  NOTE: all arguments are error checked and will return an error if there are not 4 arguments or if they are not integers.
