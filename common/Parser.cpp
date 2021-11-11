#include "RationalNumber.h"
#include "AST.h"
#include "Parser.h"
#include <iostream>
#include <memory>
#include <iomanip>

#define DEBUG 0

#if DEBUG
static int gap;
#endif


//========================================================================================
// 分析建AST用函数
// 分析中出错，统一返回 nullptr
//========================================================================================

static int CurTok;
static int getNextToken()
{
    return CurTok = Lexer::gettok();
}


void Parser::init(const std::string & InputStr)
{
#if DEBUG
gap=0;
#endif


    Lexer::init(InputStr);
    getNextToken();
}



int Parser::GetTokPrecedence()
{
    if (CurTok != Lexer::tok_op)
        return -1;
    int Op = Lexer::getOp();
    if (Op=='+' || Op=='-')
        return 10;
    if (Op=='*' || Op=='/')
        return 20;
    if (Op=='^')
        return 30;
    return -1;
}






std::unique_ptr<ExprAST> Parser::LogError(const char *Str)
{
    std::cerr <<"Parser::LogError: "<<Str <<'\n';
    exit(-1);
    return nullptr;
}


std::unique_ptr<ExprAST> Parser::ParseNumberExpr()
{

#if DEBUG
    std::cerr <<std::setw(gap) <<"" <<"ParseNumberExpr : " <<Lexer::getNumber() <<'\n';
#endif

    auto ret = std::make_unique<NumberExprAST>(Lexer::getNumber());

    getNextToken(); // eat the number

    return move(ret);
}


std::unique_ptr<ExprAST> Parser::ParseParenExpr()
{
#if DEBUG
    std::cerr <<std::setw(gap) <<"" <<"ParseParenExpr (" <<'\n';
    gap+=4;
#endif
    getNextToken(); // eat '('

    auto V = ParseExpression();
    if (!V)
        return nullptr;
    if (CurTok != Lexer::tok_rparen)
        return LogError("expected ')'");
    
    getNextToken(); // eat ')'
#if DEBUG
    gap-=4;
    std::cerr <<std::setw(gap) <<""<<")" <<'\n';
#endif
    return V;
}


std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr()
{
#if DEBUG
    std::cerr <<std::setw(gap) <<""<<"ParseIdentifierExpr : " <<Lexer::getIdentifier() <<'\n';
#endif
    std::string IdName = Lexer::getIdentifier();

    getNextToken(); // eat identifier
    return std::make_unique<VariableExprAST>(IdName);
}


std::unique_ptr<ExprAST> Parser::ParsePrimary()
{
#if DEBUG
    std::cerr <<std::setw(gap) <<"" <<"~ParsePrimary" <<'\n';
#endif    
    switch(CurTok) {
        case Lexer::tok_identifier:
            return ParseIdentifierExpr();
        case Lexer::tok_number:
            return ParseNumberExpr();
        case Lexer::tok_lparen:
            return ParseParenExpr();
        default:
            return LogError("unknown token when expecting an expression");
    }
}



std::unique_ptr<ExprAST> Parser::ParseExpression()
{
#if DEBUG
    std::cerr <<std::setw(gap) <<"" <<"~~ParseExpression" <<'\n';
#endif  
    auto LHS = ParsePrimary();
    if (!LHS)
        return nullptr;
    
    return ParseBinOpRHS(0, std::move(LHS));
}



std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS)
{
    // 一直分析直到不能分析为止

#if DEBUG
gap+=4;
#endif

    while (true) {
        int TokPrec = GetTokPrecedence();
        // 若新操作符优先级比当前小（或压根不是操作符），则结束。
        if (TokPrec < ExprPrec) {
#if DEBUG
gap-=4;
#endif
            return LHS;
        }

#if DEBUG
std::cerr <<std::setw(gap) <<"" <<"ParseBinOpRHS: " <<(char)Lexer::getOp() <<'\n';
#endif
        
        int BinOp = Lexer::getOp();
        getNextToken(); // eat op

        // 分析紧跟着的那个 RHS
        auto RHS = ParsePrimary();
        if (!RHS)
            return nullptr;
        
        // 当类似 LHS + RHS * xxx 时，才需要继续分析更新 RHS
        int NextPrec = GetTokPrecedence();
        if (TokPrec < NextPrec) {
            RHS = ParseBinOpRHS(NextPrec, move(RHS));
            if (!RHS)
                return nullptr;
        }

#if DEBUG
std::cerr <<std::setw(gap) <<"" <<"(Merge LHS RHS " <<(char)BinOp <<")\n";
#endif
        // Merge LHS RHS
        LHS = std::make_unique<OpExprAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}