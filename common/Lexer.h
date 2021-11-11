#ifndef HEHEPIG_LEXER
#define HEHEPIG_LEXER

#include "RationalNumber.h"
#include <string>

namespace Lexer {

    enum enumToken {
        tok_default = -1,
        tok_eof = -2,

        // primary
        tok_identifier = -3,
        tok_number = -4,

        tok_op = -5,    // + - * / ^

        tok_lparen = -6,   // '('
        tok_rparen = -7   // ')'
    };


    void init(const std::string &Str);

    int gettok();   // 下一个
    int curtok();   // 这一个

    const RationalNumber& getNumber();
    const std::string& getIdentifier();
    const int getOp();

    inline std::string op_to_string(int op)
    {
        std::string ret;
        ret += char(op);
        return ret;
    }
}




#endif