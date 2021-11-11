#include "./common/Parser.h"
#include "./common/Lexer.h"
#include <iostream>
#include <string>
using namespace std;


static string Str;



int PrintAST(const unique_ptr<ExprAST> &X)
{
    int ret=0;

    //puts("@Origin begin@!!");
    //X->dfsPrint();
    //cout <<"Origin:                           " <<X->to_string() <<endl;
    //puts("\n@Origin end@");

    //puts("@simplify_TransformingNegatives begin@!!");
    ret |= X->simplify_TransformingNegatives();
    //X->dfsPrint();
    //cout <<"TransformingNegatives:            " <<X->to_string() <<endl;
    //puts("\n@simplify_TransformingNegatives end@");

    //puts("@simplify_LevelingOperators begin@!!");
    ret |= X->simplify_LevelingOperators();
    //X->dfsPrint();
    //cout <<"LevelingOperators:                " <<X->to_string() <<endl;
    //puts("\n@simplify_LevelingOperators end@");

    ret |= X->simplify_SimplifyingRationalExpressions();
    //cout <<"SimplifyingRationalExpressions:   " <<X->to_string() <<endl;

    //cout <<endl;

    return ret;
}

void test()
{
    cerr <<"ready> ";
    getline(cin, Str);
    Parser::init(Str);

    unique_ptr<ExprAST> Root;

    while (1) {
        switch(Lexer::curtok()) {
            case Lexer::tok_eof:
                return;
            default:
                Root = move(Parser::ParseExpression());
        }

        if (Root) {
            cout <<endl;
            cout <<"Before:     "<<Root->to_string() <<endl;
            while (PrintAST(Root))
                continue;
            cout <<"After:      "<<Root->to_string() <<endl;
            cout <<endl;
        }
    }
}


int main()
{
    while (1) {
        test();
    }
}