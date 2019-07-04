
/*
 * HU Qiyun   3035449396
 * LIU Yunhao 3035448079
*/

#include <iostream>
#include <vector>
#include "History.h"

//#define HIS_DEBUG

bool compare ( const Command & a , const Command & b ){
  return a.executionTime > b.executionTime;
}

/**
 * brief: add a new entry into the History list
 * param: content: content of the command
 *        time:    elasped time of that command
 * reval:
*/
void History::add(std::string content , double time ){
  Command c;
  c.content = content;
  c.executionTime = time;
  command_vector.push_back(c);
}

#ifdef HIS_DEBUG
  void History::display()
  {
    std::cout<<"Histroy::display is called!\n";
  }
  void History::display_by_time()
  {
    std::cout<<"History:;display_by_time is called!\n";
  }
#else
/**
 * brief: show the most recnet 5 command
 * param: itr:  an iterator to interate through the list to get the most recent ones
 *        count: count whether #output commands is 5 already
 *        com_ptr: command, to be check whether the command is merely command
 * reval:
*/
void History::display(char * com_ptr) 
{
  if(length(com_ptr)>7)
  {
    std::cout<<"Not available:";
    int i = 7;
    while(com_ptr[i]!='\0')
    {
      std::cout<<com_ptr[i];
      ++i;
    }
    std::cout<<std::endl;
    return;
  }
  std::vector <Command> ::const_iterator itr;
  int count = 0;
  //newest lowest
  for( itr = command_vector.end()-1 ; itr != command_vector.begin()-1 && count<5 ; itr-- )
  {
    std::cout << itr->content<<"  "<<itr->executionTime << "s" << std::endl;
    count++;
  }
}

/**
 * brief: display the commands by time
 * param: itr:  an iterator to interate through the list to get the most recent ones
 *        count:count whether #output commands is 5 already
 *        c: a Commnand object to store the first 5 or less for sorting 
 * reval: 
*/
void History::display_by_time() 
{
  std::vector <Command> first5Vector; // contain first 5 command info
  std::vector <Command> :: iterator itr;

  int count = 0;
  for ( itr = command_vector.end()-1 ; itr!= command_vector.begin()-1 && count < 5; itr-- )
  {
    Command c;
    c.content = (*itr).content;
    c.executionTime = (*itr).executionTime;
    first5Vector.push_back(c);
    count ++ ;
  }
  sort( first5Vector.begin() , first5Vector.end() , compare );
  for ( itr = first5Vector.begin() ; itr != first5Vector.end() ; itr++ )
    std::cout << (*itr).content << "  "  << (*itr).executionTime << "s" << std::endl;
}
#endif

/**
 * brief: count the length of a c-string
 * param: arr: the given c-string
 * reval: an int storing the length
*/
int length(char * arr)
{
  int i = 0;
  while(arr[i]!='\0')
  {
    i++;
  }
  return i;
}

/**
 * brief: check whether a c-string contains "-sbu" as it substring
 * param: arr: the given c-string
 * reval: true if it contains 
 *        false otherwise
*/
bool contain_sbu(char * com_ptr){
  int l = length(com_ptr);
  bool contain = false;
  for ( int i=0 ; l==12 && i+3<l ; i++){
    if ( com_ptr[i] == '-' && com_ptr[i+1] == 's' && com_ptr[i+2] == 'b' && com_ptr[i+3] == 'u'){
      contain = true;
      break;
    }
  }
  return contain;
}
