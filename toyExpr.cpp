// 参考 https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl02.html
#include <cstdio>
#include <vector>
using namespace std;

//====================================================
// Lexer 词法分析
//====================================================

enum TokenType {
    tok_eof = -1,

    // commands
    tok_def = -2,
    tok_extern = -3,

    //primary
    tok_identifier = -4,
    tok_number = -5
}

static string IdentifierStr;
static double NumVal;


static int gettok() {
    static int LastChar = ' ';

    // 跳过空白符
    while (isspace(LastChar))
        LastChar = getchar();
}