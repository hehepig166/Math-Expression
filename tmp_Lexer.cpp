#include "./common/Lexer.h"
#include <iostream>
using namespace std;



void test()
{
    string Str;
    getline(cin, Str);

    Lexer::init(Str);

    while (Lexer::gettok()!=Lexer::tok_eof) {
        int tok=Lexer::curtok();

        switch(tok) {
            case Lexer::tok_identifier:
                cout <<"tok_identifier  :  " <<Lexer::getIdentifier() <<endl;
                break;
            case Lexer::tok_number:
                cout <<"tok_number      :  " <<Lexer::getNumber() <<endl;
                break;
            case Lexer::tok_op:
                cout <<"tok_op          :  " <<(char)Lexer::getOp() <<endl;
                break;
            default:
                cout <<"$unknown$" <<endl;
                return;
        }

    }
}


int main()
{
    while (1) {
        test();
    }

    return 0;
}