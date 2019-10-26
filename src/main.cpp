#include "main.h"
#include "cmd.h"
#include "parser.h"

#include <vector>

int main()
{
    parser _parser;
    while(true)
    {
        _parser.get_input();
        _parser.tokenizer();
        if(_parser.is_empty_input())
            continue;//skip the rest
        vector<cmd>cmds =  _parser();
        pid_t * pids = new pid_t[cmds.size()];
        int * results = new int[cmds.size()];
        int * pipes = new int [cmds.size()*2];
        //TODO_1 cannot execute the command             DONE!
        //TODO_1+ cannot skip empty command             DONE!
        //TODO_2 shell process & other output together
        //TODO_3 no implement of the exit               BUGGY?
        //TODO_4 parsing erro with no exception thrown
        for(unsigned int i=0; i<cmds.size(); i++)
        {
            saferpipe(pipes+(2*i));
            pid_t fork_result = saferfork();
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
                for(unsigned int j=0; j<i; j++)
                    waitpid(pids[j],results+j, WNOHANG);
                if(cmd::Seq==cmds[i].get_relation2next())
                    waitpid(pids[i], results+i, 0);
                    //HANG until this is finished
                else
                    waitpid(pids[i], results+i, WNOHANG);
                    
            }
        }
        for(unsigned int i=0; i<cmds.size(); i++)
            if(TERMINATOR==results[i]) release_exit(pids, pipes, results);
        delete [] pids;
        delete [] pipes;
        delete [] results;
    }
    return 0;
}
