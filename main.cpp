
/*
 * HU Qiyun   3035449396
 * LIU Yunhao 3035448079
*/

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <ctime>
#include "History.h"
#include "Command.h"
using namespace std;

//#define DEBUG
#define DOUBLE_BYTE           8
#define ELASPED_TIME(x,y)     (double(x-y)/CLOCKS_PER_SEC)
#define CMD_CONTAIN_STOP(cmd) (cmd[0]=='e'&&cmd[1]=='x'&&cmd[2]=='i'&&cmd[3]=='t')
#define CMD_CONTAIN_HIST(cmd) (cmd[0]=='h'&&cmd[1]=='i'&&cmd[2]=='s'&&cmd[3]=='t'&&cmd[4]=='o'&&cmd[5]=='r'&&cmd[6]=='y')

char* space_dealler(string &);


int main()
{
  History h;
  string command="";
  cout<<"tinyshell>";
  while(1)
  {
    
    getline(cin,command);
    char * com_ptr = space_dealler(command);

    #ifdef DEBUG
      printf("%s\n",com_ptr);
    #else

    if(CMD_CONTAIN_STOP(com_ptr)) {exit(0);}
    else
    {
      int fd[2]; // where fd[0] is output side of pipe, f[1] is input side
      if(pipe(fd)<0)
      {
        cerr<<"Fail to create a pipe"<<endl;
        return 1;
      }

      // create a branch, fpid is the process identification of parent process and  child process
      pid_t fpid = fork();

      // in father process, built an entry, taking information from child process, collecting the information
      if(fpid>0)
      { 
        close(fd[1]); // close the input side of pipe
        double elapsed_secs=0.0;
        read(fd[0],&elapsed_secs,DOUBLE_BYTE); // read time from pipe
        h.add(command, elapsed_secs); // collect information
      }

      // in child process, executing the command
      else if(fpid==0)
      {
        close(fd[0]); // close the output side of pipe
        clock_t begin = clock();
        if((length(com_ptr)>=7) && CMD_CONTAIN_HIST(com_ptr))
        {
          history_dealer(h,com_ptr);
        }
        else                          system(com_ptr);
        double time_spent = ELASPED_TIME(clock(),begin);
        write(fd[1],&time_spent,DOUBLE_BYTE); // write "time_spent" into pipe
        cout<<"tinyshell>";
        exit(0);
      }
      else
      {
        cerr<<"Fail to create a child process\n";
        return 1;
      }
    }
    #endif
    delete []com_ptr;
  }
  return 0;
}

/*
 * breif: this function is used to elimate the spare space before each line of command
 * param: str:     the input string which may contain space at the start
 *        re:      the string in which the space at the start is elimated
 * reval: re
*/
char* space_dealler(string & str)
{
  string re(1,str[0]);
  //first change all of the multiple-paces into single-space
  for(int i=1;i<str.length();i++)
  {
    if(str[i]==' ' && str[i-1]==' '){}      //do nothing, just skip this space
    else
    {
      re+=str[i];
    }
  }
  //then eliminate the space at the beginning and ending
  if(re[0]==' ')                  re.erase(0,1);
  if(re[re.length()-1]==' ')      re.erase(re.length()-1,1);
  char * cmd = new char [re.length()+1];
  re.copy(cmd,re.length());
  cmd[re.length()]='\0';
  return cmd;
}
