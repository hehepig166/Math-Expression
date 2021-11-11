#ifndef HEHEPIG_AST
#define HEHEPIG_AST

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "RationalNumber.h"
#include "Lexer.h"




//========================================================================================
// 分析树节点类
//========================================================================================


// ExprAST : 所有表达式节点的基类
class ExprAST {
public:
    virtual ~ExprAST() = default;

    // 节点转字符串
    virtual void append_string(std::string &Str) const =0;     // 追加到 Str 后面
    virtual std::string to_string() const =0;    // 返回一个 string

    // 化简，返回值为是否有改变过
    virtual int simplify_TransformingNegatives() {return 0;}  // 减换加，因为加法有交换律
    virtual int simplify_LevelingOperators() {return 0;}  // 利用结合性减少深度
    virtual int simplify_SimplifyingRationalExpressions() {return 0;}  // 化简分式，使得除法节点不为 乘/除法节点 的儿子

    // 杂
    virtual int get_op() const {return -1;}  // 若是 Op 节点，则返回 OpID，否则返回 -1
    virtual std::vector<std::unique_ptr<ExprAST>> *get_Son() {return NULL;}  // 若是 OpExprAST，返回 Son 的指针，否则返回 NULL

    // 调试用
    virtual void dfsPrint() const {}
};



// ExprNumber : 单一个有理数，如 "0.5"
class NumberExprAST : public ExprAST {
protected:
    RationalNumber Val;
public:
    NumberExprAST(const RationalNumber &Val_) : Val(Val_) {}

    void append_string(std::string &Str) const;
    std::string to_string() const;

};


// VariableExprAST : 单一个变量名，如 "a", "Pi"
class VariableExprAST : public ExprAST {
protected:
    std::string Name;
public:
    VariableExprAST(const std::string &Name_) : Name(Name_) {}

    void append_string(std::string &Str) const;
    std::string to_string() const;

};


// OpExprAST : 操作符表达式，如 "5+1+7"
// 说明：由于操作符基本都是二元或一元，所以构造的时候只传两个节点做参数，
//   而在化简之后可能出现一个节点有多个儿子的情况，故用 vector 来存储儿子。
//   vector下标统一从零开始
class OpExprAST : public ExprAST {
protected:
    int Op;
    std::vector<std::unique_ptr<ExprAST>> Son;
public:
    OpExprAST(int Op_, std::unique_ptr<ExprAST> LHS_, std::unique_ptr<ExprAST> RHS_)
    {
        Op = Op_;
        Son.clear();
        Son.push_back(std::move(LHS_));
        Son.push_back(std::move(RHS_));
    }

    void append_string(std::string &Str) const;
    std::string to_string() const;

    int simplify_TransformingNegatives();
    int simplify_LevelingOperators();
    int simplify_SimplifyingRationalExpressions();

    int get_op() const {return Op;}
    std::vector<std::unique_ptr<ExprAST>> *get_Son() {return &Son;}

    void dfsPrint() const;
};











#endif