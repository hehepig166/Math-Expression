#include "Lexer.h"
#include "RationalNumber.h"
#include <string>
using namespace std;

static string InputStr;
static int InputLen;
static int CurPtr;
static int CurTok;
static int LastChar;

static RationalNumber NumVal;
static string IdentifierStr;
static int OpID;

static const int EOFchar = 0;

static int getNxtChar()
{
    if (CurPtr < InputLen)
        return InputStr[CurPtr++];
    return EOFchar;
}


void Lexer::init(const string &Str)
{
    InputStr = Str;
    InputLen = Str.length();
    CurPtr = 0;
    CurTok = tok_default;
    LastChar = ' ';
}


int Lexer::gettok()
{
    while (isspace(LastChar))
        LastChar = getNxtChar();

    // identifier: (a-zA-Z)(a-zA-Z0-9)*
    if (isalpha(LastChar)) {
        IdentifierStr = LastChar;
        while (isalnum((LastChar = getNxtChar())))
            IdentifierStr += LastChar;
        
        return CurTok = tok_identifier;
    }

    // RationalNumber: ((0-9)* '.' (0-9)*) | (0-9)*
    if (isdigit(LastChar) || LastChar == '.') {
        CurPtr--;
        NumVal.init(InputStr, CurPtr);
        LastChar = ' ';
        return CurTok = tok_number;
    }

    // lparen: '('
    if (LastChar == '(') {
        LastChar = getNxtChar();
        return tok_lparen;
    }

    // rparen: ')'
    if (LastChar == ')') {
        LastChar = getNxtChar();
        return tok_rparen;
    }

    // end of file
    if (LastChar == EOFchar) {
        return CurTok = tok_eof;
    }
    
    // op
    if (string("+-*/^").find(LastChar) != string::npos) {
        OpID = LastChar;
        LastChar = getNxtChar();
        return CurTok = tok_op;
    }

    std::cerr <<"(Lexer)LogError: unacceptable charactor : [" <<(char)LastChar <<"]\n";
    return CurTok = tok_eof;
}

int Lexer::curtok()
{
    return CurTok;
}



const RationalNumber& Lexer::getNumber()
{
    return NumVal;
}

const string& Lexer::getIdentifier()
{
    return IdentifierStr;
}

const int Lexer::getOp()
{
    return OpID;
}