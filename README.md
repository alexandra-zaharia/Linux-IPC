# Linux-IPC
Learning Linux inter-process communication (IPC)

## Resources used
* "Linux Inter Process Communication (IPC) from Scratch" course on [Udemy](https://www.udemy.com/linuxipc) by Abhishek Sagar
* [The Linux Programming Interface](http://man7.org/tlpi) by Michael Kerrisk

## Structure
This repo contains several programs, organized as follows:

* `unix_domain_sockets` -- client/server implementation using stream-based UNIX domain sockets
  * `no_multiplexing` -- in this implementation, the server is not multiplexed, meaning it can only serve one client at a time

## Compiling and running the code examples
Binaries for all programs are provided in the [`bin`](https://github.com/alexandra-zaharia/Linux-IPC/tree/master/bin) directory.

Alternatively, they can be compiled using `cmake` and `make`, as follows:

```
git clone https://github.com/alexandra-zaharia/Linux-IPC.git 
cd Linux-IPC
mkdir build
cd build
cmake make .. -DCMAKE_INSTALL_PREFIX=../bin 
make && make install
cd ../bin
```

From `bin`, `cd` into the desired directory and execute the binary files.
