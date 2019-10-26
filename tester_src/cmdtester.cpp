#include "../src/cmd.h"
#include <cstring>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>

const string TESTER_NAME = "testing_1";
const string TESTER_ARG0 = "arg_0";
const string TESTER_ARG1 = "arg_1";
const string TESTER_ARG2 = "arg_2";
const string TESTER_ARG3 = "arg_3";


int main()
{
    #ifdef PRIM_DEBUG
    //test constructor
    char * tester_name = new char [TESTER_NAME.length()+1];
    memcpy(tester_name, TESTER_NAME.data(), TESTER_NAME.length()+1);

    char ** argvs = new char * [5];
    argvs[4] = nullptr;
    for(int i=0; i<4; i++)
    {
        argvs[i] = new char [6];
    }
    memcpy(argvs[0],TESTER_ARG0.data(), 6);
    memcpy(argvs[1],TESTER_ARG1.data(), 6);
    memcpy(argvs[2],TESTER_ARG2.data(), 6);
    memcpy(argvs[3],TESTER_ARG3.data(), 6);

    std::vector<cmd> _cmds;

    cmd _cmd_1 = cmd(tester_name, argvs, 5, NO_REDIRCT, NO_REDIRCT, cmd::Pipe);
    char * tester_nam2 = new char [TESTER_NAME.length()+1];
    memcpy(tester_nam2, tester_name, 10);
    tester_nam2[8]='2';
    cmd _cmd_2 = cmd(tester_nam2, argvs, 5, NO_REDIRCT, NO_REDIRCT, cmd::Seq);
    cmd _cmd_3 = _cmd_2;

    _cmds.push_back(_cmd_1);
    _cmds.push_back(_cmd_2);
    _cmds.push_back(_cmd_3);

    for(auto _cmd:_cmds)
        _cmd.__testing_print();

    int * _pipe = new int [6];
    pid_t * _pids = new pid_t [3];
    for(int i=0; i<6; i++)
        if(pipe(_pipe+i*2)<0)
            std::cerr<<"Pipe failed"<<std::endl;

    int dead_pid = _cmds.size();
    for(unsigned int i=0; i<_cmds.size();i++)
    {
        pid_t fork_result = fork();
        if(fork_result<0)
        {
            std::cerr<<"FORK ERROR"<<std::endl;
        }
        else if(fork_result==0)
        {
            if(0==i)
                _cmds[i].__testing_execu(-1, _pipe[2*i+1], nullptr);
            else if(_cmds.size()-1==i)
                _cmds[i].__testing_execu(_pipe[2*i-2], -1, &(_cmds[i-1]));
            else
                _cmds[i].__testing_execu(_pipe[2*i-2],_pipe[2*i+1], &(_cmds[i-1]));
            exit(0);
        }
        else
        {
            _pids[i] = fork_result;
            for(unsigned int j=0;j<i;j++)
            {
                if(_pids[j])
                {
                    pid_t end = waitpid(_pids[j], NULL, WNOHANG);
                    if(end>0)
                    {
                        std::cerr<<end<<" is finised"<<std::endl;
                        _pids[j]=0;
                        dead_pid-=1;
                    }
                }
            }
            if(cmd::Seq==_cmds[i].get_relation2next())
            {
                std::cerr<<"Waiting for previous process(es) to finish"<<std::endl;
                if(_pids[i])
                {
                    pid_t end = waitpid(_pids[i], NULL, 0);
                    if(end>0)
                    {
                        std::cerr<<end<<" is finised"<<std::endl;
                        dead_pid-=1;
                        _pids[i]=0;
                    }
                }
                else    std::cerr<<"Finished already"<<std::endl;
                
            }
        }
    } 
    while(dead_pid>0)
    {
        pid_t end = waitpid(-1, NULL, WNOHANG);
        if(end>0)
        {
            std::cerr<<end<<" is finised"<<std::endl;
            dead_pid-=1;
        }       
    }

    delete [] tester_nam2;
    delete [] tester_name;
    for(int i=0; i<5; i++)
        if(argvs[i]!=nullptr)
            delete [] argvs[i];
    delete [] argvs;
    #else
    std::cout<<"This is only for debugging mode\nDeubgging mode is shut down"<<std::endl;
    #endif
    return 0;
}