# POSIX shared memory

Shared memory is a block of memory that is simultaneously accessible by several processes. There may be only one process that writes data into a shared memory block, whereas several processes may read the same shared memory block.

## Client/server communication

The writer process accepts keyboard input from the user and expects integers. Whenever an integer is received, the writer process writes its value into a shared memory region designated by `/shm_test`. The reader process or processes read(s) the data from `/shm_test` and display(s) it on STDOUT as integers. 

The writer process disconnects when:
  * the user enters the value 0 
  * the user presses Ctrl+D (EOF, handled as SIGINT)
  * the user presses Ctrl+C (SIGINT)
  
The reader process disconnects when the user presses Ctrl+C.