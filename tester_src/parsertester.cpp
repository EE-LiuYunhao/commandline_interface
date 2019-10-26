#include "../src/parser.h"

int main()
{
    #ifdef DEBUG
    parser _parser;
    // Test one, get input
    _parser.get_input();
    _parser.testing_print();

    cout<<"This time is for cin\n$>";
    cin>>_parser;
    _parser.testing_print();

    _parser.tokenizer();
    _parser();

    _parser("ls -al -Rr &  & grep < a.out > b.out");
    #else
    cout<<"This is only for debugging mode\nDeubgging mode is shut down"<<std::endl;
    #endif

    return 0;
}