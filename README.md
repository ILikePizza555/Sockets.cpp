[![Build Status](https://travis-ci.org/ILikePizza555/Sockets.cpp.svg?branch=master)](https://travis-ci.org/ILikePizza555/Sockets.cpp)
Sockets.cpp is a cross-platform socket library written in C++. 

Sockets only supports synchronous TCP/UDP sockets. Asynchronous network programming is better handled by other libraries and are therefore outside the scope of this project. Additional protocols maybe supported in the future.

# Requirements

- An operating system
- A C++14 capable compiler
- CMake

# Compiling

1. Configure using CMake
```
$ mkdir build
$ cd build
$ cmake -G "Unix Makefiles" ..
```

2. Run make
```
$ make
```
