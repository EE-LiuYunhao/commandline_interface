#include "cmd.h"
#include "main.h"

/**
 * Assign the value of an fd to an fd_t variable
 * If negative and not predefined value: return NO_REIRCT
 * @param: entered fd
 * @reval: the entered fd if it is in [0, INFINITY)
 *         NO_REDIRCT otherwise
*/
inline fd_t protect_assign(fd_t _fd)
{
    return _fd < NO_REDIRCT ? NO_REDIRCT : _fd;
}

/**
 * Copy construction
 * 
 * @param: copier --- the obj to be copied
 * @reval: -/-
 */
cmd::cmd(const cmd & copier)
{
    cmd_name = new char [strlen(copier.cmd_name)+1];
    memcpy(cmd_name, copier.cmd_name, strlen(copier.cmd_name)+1);

    argvc = copier.argvc;
    argv_lst = new char * [argvc];
    for(int i=0;i<argvc;i++)
    {
        if(nullptr == copier.argv_lst[i])
            argv_lst[i] = nullptr;
        else
        {
            argv_lst[i] = new char [strlen(copier.argv_lst[i])+1];
            mempcpy(argv_lst[i],\
                    copier.argv_lst[i],\
                    strlen(copier.argv_lst[i])+1);
        }
    }
    iFile = protect_assign(copier.iFile);
    oFile = protect_assign(copier.oFile);
    relation_next = copier.relation_next;
}

/**
 * Syntax move copy construction
 * 
 * @param: copier --- the obj to be copied
 * @reval: -/-
 */
cmd::cmd(cmd && copier)
{
    cmd_name = copier.cmd_name;
    copier.cmd_name = nullptr;

    argv_lst = copier.argv_lst;
    copier.argv_lst=nullptr;

    argvc = copier.argvc;
    iFile = protect_assign(copier.iFile);
    oFile = protect_assign(copier.oFile);
    relation_next = copier.relation_next;
}

/**
 * Assigning operator
 * 
 * @param: copier --- the obj to be assigned to this
 * @reval: a reference to this
 */
cmd & cmd::operator=(const cmd & copier)
{
    delete [] cmd_name;
    if(argv_lst!=nullptr)
    {
        for(int i=0; i<argvc; i++)
            delete [] argv_lst[i];
        delete argv_lst;
    }

    cmd_name = new char [strlen(copier.cmd_name)+1];
    memcpy(cmd_name, copier.cmd_name, strlen(copier.cmd_name)+1);

    argvc = copier.argvc;
    argv_lst = new char * [argvc];
    for(int i=0;i<argvc;i++)
    {
        if(nullptr == copier.argv_lst[i])
            argv_lst[i] = nullptr;
        else
        {
            argv_lst[i] = new char [strlen(copier.argv_lst[i])+1];
            mempcpy(argv_lst[i],\
                    copier.argv_lst[i],\
                    strlen(copier.argv_lst[i])+1);
        }
    }
    iFile = protect_assign(copier.iFile);
    oFile = protect_assign(copier.oFile);
    relation_next = copier.relation_next;

    return *this;
}

/**
 * Syntax movement assigning operator
 * 
 * @param: copier --- the obj to be assigned to this 
 * @reval: an reference to this obj
*/
cmd & cmd::operator=(cmd && copier)
{
    delete [] cmd_name;
    if(argv_lst!=nullptr)
    {
        for(int i=0; i<argvc; i++)
            delete [] argv_lst[i];
        delete argv_lst;
    }

    cmd_name = copier.cmd_name;
    copier.cmd_name = nullptr;

    argv_lst = copier.argv_lst;
    copier.argv_lst=nullptr;

    argvc = copier.argvc;
    iFile = protect_assign(copier.iFile);
    oFile = protect_assign(copier.oFile);
    relation_next = copier.relation_next;

    return *this;
}


#ifdef PRIM_DEBUG
    /**
     * For testing whether the command name and arguments
     * are stored correctly.
     * 
     * @param: void
     * @reval: void
    */
    void cmd::__testing_print() const
    {
        using std::cout; 
        using std::endl;

        cout<<cmd_name<<'\t';
        if(argvc==0) return;
        for(int i=0; i<argvc-1; i++)
            cout<<argv_lst[i]<<' ';
        cout<<endl;
    }

    /**
     * For testing whether the command name and arguments
     * are to be executed correctly.
     * 
     * @param: _pipe_i --- the read end of the pipe
     *         _pipe_o --- the write end of the pipe
     *         prev --- pointer to the command to be executed before it
     * @reval: void
    */
    void cmd::__testing_execu(int _pipe_i, int _pipe_o, const cmd * prev) const
    {
        cout<<"[execution] ";
        if(iFile!=NO_REDIRCT) dup2(iFile, STDIN_FILENO);
        if(oFile!=NO_REDIRCT) dup2(oFile, STDOUT_FILENO);
        if(prev!=nullptr && prev->relation_next == cmd::Pipe)
        {
            if(NO_REDIRCT==iFile)
            {
                safterdup2(_pipe_i, STDIN_FILENO);
                cerr<<cmd_name<<" in redirected reading>>>>";
                string temp;
                cin>>temp;
                cout<<"len="<<temp.length()<<" content=\""<<temp<<'\"'<<endl;
            }
            else
                cerr<<"BOTH PIPE AND REDIRCT"<<endl;
        }
        close(_pipe_i);
        switch(relation_next)
        {
            case Pipe:
                safterdup2(_pipe_o, STDOUT_FILENO);
                cerr<<cmd_name<<" out redirected"<<endl;
                if(oFile!=NO_REDIRCT)
                    cerr<<"BOTH PIPE AND REDIRCT"<<endl;
                break;
            case Bgk:
                if(0==saferfork()) exit(0);
            default:
                break;
        }
        close(_pipe_o);

        //replace for the execvp...
        this->__testing_print();
    }
#endif

/**
 * Run the command. 
 * 
 * @param: _pipe_i --- read end of the pipe
 *         _pipe_o --- write end of the pipe
 *         prev --- pointer to the command to be executed before it
 * @reval:
*/
void cmd::execute(int _pipe_i, int _pipe_o, const cmd * prev)
{
    if(iFile!=NO_REDIRCT) dup2(iFile, STDIN_FILENO);
    if(oFile!=NO_REDIRCT) dup2(oFile, STDOUT_FILENO);
    if(prev!=nullptr && prev->relation_next == cmd::Pipe)
    {
        if(-1==iFile)
            safterdup2(_pipe_i, STDIN_FILENO);
        else
            cerr<<"BOTH PIPE AND REDIRCT"<<endl;
    }
    close(_pipe_i);
    switch(relation_next)
    {
        case Pipe:
            safterdup2(_pipe_o, STDOUT_FILENO);
            if(oFile!=NO_REDIRCT)
                cerr<<"BOTH PIPE AND REDIRCT"<<endl;
            break;
        case Bgk:
            if(0!=saferfork()) exit(0);
        default:
            break;
    }
    close(_pipe_o);
    execvp(cmd_name,argv_lst);
    exit(0);
}

/**
 * Convert the given file name to be read from to FD
 * 
 * @param: a string referring to the file name (path)
 * @reval: a file descriptor
 */ 
fd_t cmd::iFd_open(const string & iFile_name)
{
    fd_t iFd = open(iFile_name.data(),O_RDONLY);
    this->iFile = protect_assign(iFd);
    return iFd;
}

/**
 * Convert the given file name to be writen to to FD
 * 
 * @param: a string referring to the file name (path)
 * @reval: a file descriptor
 */ 
fd_t cmd::oFd_open(const string & oFile_name)
{
    fd_t oFd = open(oFile_name.data(),O_WRONLY);
    this->oFile = protect_assign(oFd);
    return oFd;
}

/**
 * Check whether the cmd is a cd and change the directory if so
 * 
 * @param: _cmd ---- const reference to one cmd object
 * @reval: whether the cmd is a cd
 */ 
bool change_dir(const cmd & _cmd)
{
    if(0==strcmp(_cmd.cmd_name, "cd"))
    {
        if(2==_cmd.argvc)
        {
            if(chdir(_cmd.argv_lst[1]))
                cerr<<"Not a directory"<<endl;
        }
        else if(1==_cmd.argvc)
        {
            if(chdir(getenv("HOME")))
                cerr<<"Not a directory"<<endl;
        }
        else
            cerr<<"Parser error around \"cd\""<<endl;
    }
    else
        return false;
    return true;
}
/**
 * Check whether the cmd is a exit
 * 
 * @param: void
 * @reval: 0 if the command is not exit
 *         TERMINATOR otherwise
 */ 
int check_exit(const cmd & _cmd)
{
    if(0==memcmp(_cmd.cmd_name, "exit", 4))
        return TERMINATOR;
    else
        return 0;
}

