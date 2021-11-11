#ifndef HEHEPIG_PARSER
#define HEHEPIG_PARSER

#include <string>
#include <vector>
#include <memory>
#include "AST.h"



//========================================================================================
// 分析建AST用函数
// 分析中出错，统一返回 nullptr
//========================================================================================
namespace Parser {

    // 初始化
    void init(const std::string &InputStr);

    // LogError
    std::unique_ptr<ExprAST> LogError(const char *Str);

    // 若当前是操作符，则返回当前操作符优先级，否则返回 -1
    int GetTokPrecedence();


    // numberexpr
    //   ::= rationalnumber
    std::unique_ptr<ExprAST> ParseNumberExpr();

    // parenexpr
    //   ::= '(' expression ')'
    std::unique_ptr<ExprAST> ParseParenExpr();

    // identifierexpr
    //   ::= identifier
    std::unique_ptr<ExprAST> ParseIdentifierExpr();

    // primary
    //   ::= identifierexpr
    //   ::= numberexpr
    //   ::= parenexpr
    std::unique_ptr<ExprAST> ParsePrimary();




    // expression
    //   ::= primary binoprhs
    std::unique_ptr<ExprAST> ParseExpression();

    // binoprhs
    //   ::= (Op primary)*
    // Op: + - * / ^
    std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);



}






#endif