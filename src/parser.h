#ifndef _PARSER_H_
#define _PARSER_H

#include <iostream>
#include <string>
#include <vector>

// #define DEBUG

#include "cmd.h"

using std::cin;
using std::cout;
using std::cerr;
using std::string;
using std::vector;


class parser
{
public: 
    const static int NO_WHITE_SPACE = 5;
    const static int NO_SYSBOLS = 5;
    const static char WHITE_SPACE[NO_WHITE_SPACE];
    const static char SYMBOLS[NO_SYSBOLS];
private:
    string buf;
    enum connector_symbol
    {
        Input ='<', Output ='>', Pipe ='|', List =';', Bgk = '&', Seq
    };
    struct connector
    {
        int pos;
        connector_symbol sym;
    };
    vector<string> word_lst;
    vector<connector> connectors;
public:
    //External implementation
    void get_input();
    void tokenizer(); // splite the buf into words
    string get_strbuf(){return this->buf;}
    #ifndef DEBUG
    vector<cmd> operator () () const; // build an cmd object basing on current words
    vector<cmd> operator () (const string &); // build an cmd object basing on input string
    #else
    void operator () () const; // build an cmd object basing on current words
    void operator () (const string &); // build an cmd object basing on input string
    #endif

    #ifdef DEBUG
    void testing_print() const
    {
        cout<<"Testing print: buf= "<<buf<<std::endl;
    }
    #endif

    bool is_empty_input() const
    {
        return buf.size()==0;
    }

    friend std::istream & operator >> (std::istream & cin, parser &);
    #ifdef DEBUG
    friend void command_constructor(const vector<string> & words,
                                    const vector<parser::connector> & conns, 
                                    fd_t iFd,
                                    fd_t oFd,
                                    int id,
                                    int prev,
                                    int iFdid,
                                    int oFdid,
                                    cmd::symbol relation);
    #else
    friend cmd command_constructor(const vector<string> & words,
                                   const vector<parser::connector> & conns, 
                                   fd_t iFd,
                                   fd_t oFd,
                                   int id,
                                   int prev,
                                   int iFdid,
                                   int oFdid,
                                   cmd::symbol relation);
    #endif
};

//External implementation
std::istream & operator >> (std::istream & cin, parser &);

#ifdef DEBUG
void command_constructor(const vector<string> & words,
                         const vector<parser::connector> & conns, 
                         fd_t iFd,
                         fd_t oFd,
                         int id,
                         int prev,
                         int iFdid,
                         int oFdid,
                         cmd::symbol relation);
#else
cmd command_constructor(const vector<string> & words,
                        const vector<parser::connector> & conns, 
                        fd_t iFd,
                        fd_t oFd,
                        int id,
                        int prev,
                        int iFdid,
                        int oFdid,
                        cmd::symbol relation);
#endif

#endif