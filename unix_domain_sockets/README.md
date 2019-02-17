# UNIX domain sockets

One manner in which inter-process communication (IPC) can be handled is via UNIX domain sockets (for POSIX-compliant operating systems). Here are two implementations of client/server communication via sockets, where the server and client processes reside on the **same machine**:

* `no_multiplexing` -- contains a server implementation that is not multiplexed, i.e. that can only handle one client at a time;
* `multiplexing` -- contains a server implementation that is multiplexed, i.e. it can serve several client processes at once.

The [client](https://github.com/alexandra-zaharia/Linux-IPC/blob/master/unix_domain_sockets/client.c) is the same for both server implementations.

## Client/server communication

The user of a client process sends integer values to the server. When the value 0 is sent, it means the client finished sending out data and expects the sum of all the integers it has sent to the server.

## Improvements

The starting point for these two implementations are the "Linux IPC from Scratch" course on Udemy and Michael Kerrisk's "The Linux Programming Interface". With respect to these resources, the two implementations here feature the following improvements:

* The code is **refactored** into several functions instead of having everything in a huge `main` method.

* **Input validation** is performed in order to ensure that clients only send integer values to the server. Invalid values are ignored.

* The servers (with and without multiplexing) as well as the client register a **signal handler for SIGINT** (Ctrl + C) in order to shut down cleanly (closing the connection, unlinking the socket path, etc.).

* When the client receives end of file (**EOF**) through Ctrl + D, it **`raise`s SIGINT** and thus terminates cleanly.

* The servers (with and without multiplexing) as well as the client ignore SIGPIPE, which is sent when a write operation fails on a pipe or socket because there are no readers at the other end. The server needs to **ignore SIGPIPE** (received when a client disconnects) in order to handle the remaining clients or, at the very least, accept Ctrl + C from the user to shut down cleanly. The client needs to ignore SIGPIPE (received when the server disconnects) in order to shut down cleanly.

## TODO

At the moment, a client is aware that the server has disconnected only if it attempts to send out new data to the server. In this case, the client receives SIGPIPE which is ignored in order to check whether `errno` is set to EPIPE. Then the client `raise`s SIGINT, which is handled by a function that terminates the process cleanly. The best thing to do would be for the server to send out a message (via SIGUSR1 for example) to every connected client before terminating. This way, the clients would know that the server is disconnecting and they would do the same. However, this means that the server process needs to maintain a list of PIDs for each of its connected clients, that they need to send out once a connection with the server has been established.
