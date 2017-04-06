# WiFi-Synchronization-
An ad hoc wireless network written in C to demonstrate Mutex and Semaphore locking mechanisms

# Approach:
	The first thing we did was create a global array representing the 100x100 grid. This global array holds the positions of the nodes so that we can ensure no two nodes will occupy the same position on the grid.
	
	To represent the grid we created 3 different arrays (1 for each channel), and filled the cells with a struct 'broadcasts'.
	struct broadcasts had the following fields:
		id: 4 byte id number
		message: the message to send
		recast: was the message rebroadcasted?
		channel: what channel was it on

# How we handled synchronization problems:
	Initial Broadcast - When a node first decides to broadcast, it looks to make sure that it wont intercept with the zone from an adjacent node's broadcast.
	Channel lock - We used a lock on the channels so that two nodes couldn't write onto the channel at the same time.
	Trylock - We elected to use trylock rather than condition variables because we could avoid the unnecessary wait time if the node can't acquire the lock.
	Helpers - We made helper functions to check if broadcasts radius' interfered with anything, mark areas for broadcasts, choose channels, etc
	Noisemakers - Noisemakers will overwrite any broadcast in it's radius. They're lethal.

# Running the Program:
- Mutex -
1. run "make clean" & "make" to compile
2. run "./mutex" to run the program
3. the program will run with no output to the terminal (this is normal)
4. the output comes in the form of "log" files that contain the output desired in the criteria
5. the file names will be named "(node number) + Mutex_Node_Output.txt"
6. example output file: 6Mutex_Node_Output.txt
7. to stop program perform the standard ^C command in the terminal to exit

- Semaphores - 
1. run "make clean" & "make" to compile
2. run "./semaphore" to run the program
3. the program will run with no output to the terminal (this is normal)
4. the output comes in the form of "log" files that contain the output desired in the criteria
5. the file names will be named "(node number) + Semaphore_Node_Output.txt"
6. example output file: 6Semaphore_Node_Output.txt
7. to stop program perform the standard ^C command in the terminal to exit
