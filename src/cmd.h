#ifndef _CMD_H_
#define _CMD_H_

#include <iostream>
#include <string>

// #define PRIM_DEBUG

using std::string;
typedef int fd_t;

const fd_t NO_REDIRCT = -1;

class cmd
{
    public: 
        enum symbol {Pipe='|', List=';', Bgk='&', Seq, End='\0'};
    private:
    //member variables
        char * cmd_name; //heap CString
        char ** argv_lst; //heap CString Array
        int argvc;
        fd_t iFile; //fd
        fd_t oFile; //fd
        symbol relation_next;
    public: 
    //member methods
        cmd():cmd_name(nullptr),argv_lst(nullptr),argvc(0),iFile(NO_REDIRCT),oFile(NO_REDIRCT),relation_next(End){}
        cmd(char * _cmd_name, char ** _argv_lst, int _argvc, int _iFile, int _oFile, symbol _relation):\
        cmd_name(_cmd_name), argv_lst(_argv_lst), argvc(_argvc), iFile(_iFile), oFile(_oFile), relation_next(_relation)\
        {
            if(iFile<NO_REDIRCT) iFile=NO_REDIRCT;
            if(oFile<NO_REDIRCT) oFile=NO_REDIRCT;
        }
        ~cmd()
        {
            delete [] cmd_name;
            if(argv_lst!=nullptr)
            {
                for(int i=0; i<argvc; i++)
                    delete [] argv_lst[i];
                delete argv_lst;
            }
        }

        symbol get_relation2next() const
        {
            return this->relation_next;
        }
        //external defination
        cmd(const cmd &);
        cmd(cmd &&);
        cmd & operator=(const cmd & copier);
        cmd & operator=(cmd && copier);
        #ifdef PRIM_DEBUG
        void __testing_print() const;
        void __testing_execu(int _pipe_i, int _pipe_o, const cmd * prev) const;
        #endif
        void execute(int _pipe_i, int _pipe_o, const cmd * prev);

        fd_t iFd_open(const string & iFile_name);
        fd_t oFd_open(const string & oFile_name);
};

#endif
