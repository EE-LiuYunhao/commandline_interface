#include "main.h"
#include "cmd.h"
#include "parser.h"

#include <vector>

static unsigned int exitedchildren = 0;

static void childexit_handler(int signum)
{
    int r, ch_pid;
    if(SIGCHLD == signum)
        ch_pid=waitpid(-1, &r, 0);
    else
        return;
    // cerr<<"[DEBUG] One child "<<ch_pid<<" exits with ";
    // if(WIFEXITED(r))    cerr<<WEXITSTATUS(r)<<" (exit code)"<<endl;
    // if(WIFSIGNALED(r))  cerr<<WTERMSIG(r)<<" (signal num)"<<endl;
    exitedchildren += 1;
}

int main()
{
    unsigned int prev_cmdsize = 0;
    signal(SIGCHLD, childexit_handler);
    parser _parser;
    while(true)
    {
        while(exitedchildren<prev_cmdsize) sleep(1);
        _parser.get_input();
        _parser.tokenizer();
        if(_parser.is_empty_input())
            continue;//skip the rest
        vector<cmd>cmds =  _parser();
        exitedchildren = 0;
        prev_cmdsize = cmds.size();
        pid_t * pids = new pid_t[cmds.size()];
        int * results = new int[cmds.size()];
        int * pipes = new int [cmds.size()*2];
        //TODO_1 cannot execute the command             DONE!
        //TODO_1+ cannot skip empty command             DONE!
        //TODO_2 shell process & other output together  DONE!
        //TODO_3 no implement of the exit               DONE!
        //TODO_4 parsing erro with no exception thrown 
        //TODO_5 pipe infintely loop                    DONE!
        //TODO_6 cd has bug                             DONE!
        //TODO_7 < or > cannot create new file
        //TODO_7+ << or >> not supported
        for(unsigned int i=0; i<cmds.size(); i++)
        {
            saferpipe(pipes+(2*i));
            if(change_dir(cmds[i]))
            {
                pids[i]=0;
                exitedchildren += 1;
                continue;
            }
            if(TERMINATOR == check_exit(cmds[i]))
            {
                for(unsigned int j=0; j<i; j++)
                {
                    if(pids[j]!=0)
                        kill(pids[j],SIGTERM);
                }
                release_exit(pids, pipes, results);
            }
            if(0==strcmp(cmds[i].get_command_name(),"grep") || \
               0==strcmp(cmds[i].get_command_name(),"head")
            )
                exitedchildren+=1;
            pid_t fork_result = saferfork();
            //implementation of cd
            if(0==fork_result)
            {
                if(0==i)
                    cmds[i].execute(-1, pipes[2*i+1], nullptr);
                else if(static_cast<unsigned int>(cmds.size())-1==i)
                    cmds[i].execute(pipes[2*i-2], -1, &(cmds[i-1]));
                else
                    cmds[i].execute(pipes[2*i-2],pipes[2*i+1], &(cmds[i-1]));
            }
            else
            {
                pids[i] = fork_result;
                if(cmd::Seq==cmds[i].get_relation2next())
                {
                    waitpid(pids[i], results+i, 0);
                    //HANG until this is finished
                    pids[i]=0;
                }
            }
        }
        delete [] pids;
        delete [] pipes;
        delete [] results;
    }
    return 0;
}
