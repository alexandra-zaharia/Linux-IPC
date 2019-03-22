# POSIX message queues

A message queue may be established between a sender (client) and a receiving (server) process. It is the responsibility of the receiving process to create the message queues. Message queues can only be used to send strings between two processes. 

## Client/server communication

The client (sender) sends integers to the server (receiving) process, until the value 0 is reached. The server displays the message it received from the client to STDOUT. The server is **multiplexed**, meaning there may be several clients connected to it at the same time.

The client disconnects when:
* the user enters the value 0
* the user presses Ctrl+D (EOF, handled as SIGINT)
* the user presses Ctrl+C (SIGINT)

The server disconnects when the user presses Ctrl+C.