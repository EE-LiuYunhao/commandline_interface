#ifndef _MAIN_H_
#define _MAIN_H_

#include <iostream>
#include <unistd.h> //fork() execvp() pipe() dup2()
#include <cstring> //memcpy()
#include <dirent.h> //for DIR
#include <fcntl.h> //for open -> fd
#include <sys/wait.h> //for wait() waitpid()

using std::cin;
using std::cout;
using std::endl;
using std::cerr;

inline void release_exit(int * pids, int * pipes, int * results)
{
    delete [] pids;
    delete [] pipes;
    delete [] results;

    exit(0);
}

const int TERMINATOR=200;
const int FORK_ERR=511;
const int PIPE_ERR=512;
const int DUP2_ERR=513;

typedef int fd_t;

inline pid_t saferfork()
{
    pid_t ret = fork();
    if(ret<0) 
    {
        cerr<<"Fail to fork"<<endl;
        exit(FORK_ERR);
    }
    else
        return ret;
}

inline void saferpipe(int * _pipe)
{
    if(pipe(_pipe)<0)
    {
        cerr<<"Fail to pipe!"<<endl;
        exit(PIPE_ERR);
    }
}

inline void safterdup2(fd_t oldFd, fd_t newFd)
{
    if(oldFd < 0 || dup2(oldFd, newFd)<0)
    {
        cerr<<"Fail to redirct the file to "<<newFd<<"!"<<endl;
        exit(DUP2_ERR);
    }
}

#endif