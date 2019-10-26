#include "parser.h"
#include "main.h"
#include <cstring>
#include <algorithm>

const char parser::WHITE_SPACE [] = {' ','\n','\r','\t','\v'};
const char parser::SYMBOLS [] = {'<','|','>',';','&'};

/** Determine whether the tar appears in the arr
 * 
 * @param tar: the target to be find
 * @param arr: the array that may contains the tar
 * @param len: the length of the array
 * @retval true if the tar appears in th arr
 *         false otherwise
 */
bool find(char tar, const char * arr, int len)
{
    for(int i=0; i<len; i++)
    {
        if(tar == arr[i])
            return true;
    }
    return false;
}

/** Remove the whitespaces from the given string
 * 
 * @param str: the given string from which the whitespaces should be removed
 * @retval void
 */
void remove_whitespace(string & str)
{
    for(unsigned int i=0; i<str.length(); i++)
    {
        if(find(str[i],parser::WHITE_SPACE, parser::NO_WHITE_SPACE))
            str.erase(i,1);
    }
}

/** Promote the $> and receive a string from the cin buffer
 *  
 *  @param void
 *  @retval void
 */
void parser::get_input()
{
    cout<<"$>";
    buf = "";
    char temp_char[50];
    cin.getline(temp_char,50);
    buf = temp_char;
}

/** Splite the parser::buf into a vector of words and
 * remove all the white spaces in each words and
 * store the connector after each word
 * 
 * @param  void
 * @retval void
 */
void parser::tokenizer()
{
    int cnt_word = 0;
    word_lst.clear();
    connectors.clear();
    for(unsigned int i=0; i<buf.length(); i++)
    {
        string seg = "";
        while(!find(buf[i],WHITE_SPACE, NO_WHITE_SPACE) &&
              !find(buf[i],SYMBOLS, NO_SYSBOLS) && i<buf.length())
        {
            seg+=buf[i];
            i++;
        }
        if(seg.length()!=0)
        {
            word_lst.push_back(seg);
            cnt_word ++;
        }
        if(find(buf[i], SYMBOLS, NO_SYSBOLS))
        {
            parser::connector con;
            con.pos = cnt_word;
            con.sym = static_cast<parser::connector_symbol>(buf[i]);
            if(!connectors.empty()                               &&
            parser::connector_symbol::Bgk==connectors.back().sym &&
            parser::connector_symbol::Bgk==con.sym               &&
            connectors.back().pos==cnt_word)
                connectors.back().sym = parser::connector_symbol::Seq;
            else
                connectors.push_back(con);
        }
    }
    std::for_each(word_lst.begin(), word_lst.end(),remove_whitespace);
    std::for_each(connectors.begin(), connectors.end(), [](connector & conn)
         {
             //from "connector before word" to "connector after word" <= script logic
             conn.pos -= 1;
         });
}

/** Generate a set of the cmd from the input line
 *  Ought to be called after the tokenizer
 *  Will open the file as fd
 * 
 * @param  void
 * @retval a vector containing the constructed cmd object
 */
#ifndef DEBUG
vector<cmd> parser::operator() () const
#else
void parser::operator() () const
#endif
{
    #ifdef DEBUG
    for_each(word_lst.begin(),word_lst.end(),[](const string & str){cout<<str<<std::endl;});
    for_each(connectors.begin(), connectors.end(),
             [](const connector & ctr)
             {
                 cout<<"Connector after word id="<<ctr.pos<<": "\
                 <<static_cast<char>(ctr.sym)<<std::endl;
             }
    );
    #endif
    vector<cmd> ret;
    int prev = 0;
    int iFid = -1;
    int oFid = -1;
    int iFd = NO_REDIRCT;
    int oFd = NO_REDIRCT;

    for(auto conn:this->connectors)
    {
        switch(conn.sym)
        {
            case parser::Input:
                if(conn.pos+1 >= static_cast<int>(word_lst.size()) || iFid!=-1)
                {
                    cerr<<"Parser error on inputing"<<std::endl;
                }
                else
                {
                    iFid = conn.pos+1;
                    iFd = open(word_lst[iFid].data(), O_RDONLY);
                }
                break;
            case parser::Output:
                if(conn.pos+1 >= static_cast<int>(word_lst.size()) || oFid!=-1)
                {
                    cerr<<"Parser error on inputing"<<std::endl;
                }
                else
                {
                    oFid = conn.pos+1;
                    oFd = open(word_lst[oFid].data(), O_WRONLY);
                }
                break;
            case parser::Pipe:
                #ifndef DEBUG
                ret.push_back(
                #endif
                command_constructor(word_lst, connectors, iFd, oFd, conn.pos, prev, iFid, oFid, cmd::symbol::Pipe)
                #ifndef DEBUG
                );
                #else
                ;
                #endif
                prev = conn.pos+1;
                oFid = -1;
                iFid = -1;
                break;
            case parser::Bgk:
                #ifndef DEBUG
                ret.push_back(
                #endif
                command_constructor(word_lst, connectors, iFd, oFd, conn.pos, prev, iFid, oFid, cmd::symbol::Bgk)
                #ifndef DEBUG
                );
                #else
                ;
                #endif
                prev = conn.pos+1;
                oFid = -1;
                iFid = -1;
                break;
            case parser::List:
                #ifndef DEBUG
                ret.push_back(
                #endif
                command_constructor(word_lst, connectors, iFd, oFd, conn.pos, prev, iFid, oFid, cmd::symbol::List)
                #ifndef DEBUG
                );
                #else
                ;
                #endif
                prev = conn.pos+1;
                oFid = -1;
                iFid = -1;
                break;
            case parser::Seq:
                #ifndef DEBUG
                ret.push_back(
                #endif
                command_constructor(word_lst, connectors, iFd, oFd, conn.pos, prev, iFid, oFid, cmd::symbol::Seq)
                #ifndef DEBUG
                );
                #else
                ;
                #endif
                prev = conn.pos+1;
                oFid = -1;
                iFid = -1;
                break;
            default:
                break;
        }
    }
    command_constructor(word_lst, connectors, iFd, oFd, word_lst.size()-1, prev, iFid, oFid, cmd::End);
    #ifndef DEBUG
    return ret;
    #endif
}

/** Generate a set of the cmd from the input line
 *  Ought to be called after the tokenizer
 *  Will open the file as fd
 * 
 * @param  str --- the entire line
 * @retval a vector containing the constructed cmd object
 */
#ifndef DEBUG
vector<cmd> parser::operator() (const string & str)
#else
void parser::operator() (const string & str)
#endif
{
    buf = str;
    tokenizer();
    #ifndef DEBUG
    return this->operator()();
    #else
    this->operator()();
    #endif
}

std::istream & operator >> (std::istream & cin, parser & _parser)
{
    char temp_char[50];
    cin.getline(temp_char,50);
    _parser.buf = temp_char;
    return cin;
}

/** Construct a cmd object to be pushed back into the vector
 *  Allocate heap space for cmd member variables and
 *  call the constructor of the cmd object. 
 * 
 * @params words --- the entire line
 * @params conns --- the connectors within the line
 * @params  iFd  --- file descriptor of the input file
 * @params  oFd  --- file descriptor of the output file
 * @params  id   --- position where the command stops
 * @params prev  --- position where the command starts
 * @params relation --- relation for the next 
 * @retval a cmd object constructed with given arguments
 */
#ifdef DEBUG
void command_constructor(const vector<string> & words,
                        const vector<parser::connector> & conns, 
                         fd_t iFd,
                         fd_t oFd,
                         int id,
                         int prev,
                         int iFdid,
                         int oFdid,
                         cmd::symbol relation)
#else
cmd command_constructor(const vector<string> & words,
                        const vector<parser::connector> & conns, 
                        fd_t iFd,
                        fd_t oFd,
                        int id,
                        int prev,
                        int iFdid,
                        int oFdid,
                        cmd::symbol relation)
#endif
{
    using std::endl;
    
    char * command = new char [words[prev].length()+1];
    memcpy(command, words[prev].data(), words[prev].length()+1);

    int num_argvs = id - prev + 1;
    num_argvs -= (iFdid>=0)?1:0;
    num_argvs -= (oFdid>=0)?1:0;
    if(num_argvs<1)
    {
        cerr<<"Invalid arguments"<<endl;
        #ifdef DEBUG
        return;
        #else
        return cmd();
        //TODO: through an exception to the main
        #endif
    }

    char ** argvs = new char * [num_argvs+1];
    argvs[num_argvs] = nullptr;

    for(int i=prev;i<=id;i++)
    {
        if(i==iFdid || i==oFdid)    continue;
        argvs[i-prev] = new char [words[i].length()+1];
        memcpy(argvs[i-prev], words[i].data(), words[i].length()+1);
    }
    #ifdef DEBUG
    cout<<"command: "<<command<<"; argvs: "<<num_argvs<<endl;
    for(int i=0; i<num_argvs; i++)
        cout<<'\t'<<argvs[i]<<endl;
    cout<<endl;
    #else
    return cmd(command, argvs, num_argvs, iFd, oFd, relation);
    #endif
}