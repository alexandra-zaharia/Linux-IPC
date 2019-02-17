# Linux-IPC
This is a collection of mini-projects I did while learning Linux inter-process communication (IPC).

## Resources used
* "Linux Inter Process Communication (IPC) from Scratch" course on [Udemy](https://www.udemy.com/linuxipc) by Abhishek Sagar
* [The Linux Programming Interface](http://man7.org/tlpi) by Michael Kerrisk

## Structure
This repo contains several projects (each of them has a README providing more information), organized as follows:

* [`routing_table_manager`](https://github.com/alexandra-zaharia/Linux-IPC/tree/master/routing_table_manager) -- IPC via UNIX domain sockets, signals, and shared memory (the server process maintains a routing table that is synchronized across all clients)

* [`unix_domain_sockets`](https://github.com/alexandra-zaharia/Linux-IPC/tree/master/unix_domain_sockets) -- IPC via stream-based UNIX domain sockets (the server process computes sums of values that are sent out by clients)

## Compiling and running the code examples
Binaries for all programs are provided in the [`bin`](https://github.com/alexandra-zaharia/Linux-IPC/tree/master/bin) directory (for Linux x86_64).

Alternatively, they can be compiled using `cmake` and `make`, as follows:

```
git clone https://github.com/alexandra-zaharia/Linux-IPC.git 
cd Linux-IPC
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../bin 
make && make install
cd ../bin
```

From `bin`, `cd` into the desired directory and execute the binary files.
