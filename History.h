
/*
 * HU Qiyun   3035449396
 * LIU Yunhao 3035448079
*/

#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <algorithm>
#include "Command.h"

char* cut(char*,int,int);
int   length(char*);

#define history_dealer(hist,cmd)    (contain_sbu(cmd) ? hist.display_by_time() : hist.display(cmd) )

class History{
private:
  std::vector <Command> command_vector;
public:
  History(){};
  void add(std::string content , double time );
  void display(char *);
  void display_by_time();
};

int length(char * arr);
bool contain_sbu(char * com_ptr);

#endif