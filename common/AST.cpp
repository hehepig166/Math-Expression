#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "RationalNumber.h"
#include "Lexer.h"
#include "AST.h"

//========================================================================================
// ExprAST 成员函数
//========================================================================================

void NumberExprAST::append_string(std::string &Str) const
{
    Val.append_string(Str);
}


void VariableExprAST::append_string(std::string &Str) const
{
    Str += Name;
}


void OpExprAST::append_string(std::string &Str) const
{
    int n=Son.size();
    if (n==0)
        return;
    
    Str += '(';
    
    Son[0]->append_string(Str);

    if (n==1)
        return;
    
    std::string opStr = Lexer::op_to_string(Op);
    for (int i=1; i<n; i++) {
        Str += ' ';
        Str += opStr;
        Str += ' ';
        Son[i]->append_string(Str);
    }

    Str += ')';
}


std::string NumberExprAST::to_string() const
{
    std::string ret;
    this->append_string(ret);
    return ret;
}

std::string VariableExprAST::to_string() const
{
    std::string ret;
    this->append_string(ret);
    return ret;
}

std::string OpExprAST::to_string() const
{
    std::string ret;
    this->append_string(ret);
    return ret;
}




int OpExprAST::simplify_TransformingNegatives()
{
    int ret=0, flag=0, n=Son.size();    // flag 用于记录是否要给儿子乘负一
    if (this->Op == '-') {
        ret = flag =1;
        this->Op = '+';
    }
    if (n==0) // 理应不为0，但还是判一下以防万一吧
        return ret;
    Son[0]->simplify_TransformingNegatives();
    if (n==1)
        return ret;
    
    for (int i=1; i<n; i++) {
        ret |= Son[i]->simplify_TransformingNegatives();
        if (flag)
            Son[i] = std::make_unique<OpExprAST>('*', std::make_unique<NumberExprAST>(-1), std::move(Son[i]));
    }

    return ret;
}


int OpExprAST::simplify_LevelingOperators()
{
    int ret=0, flag=0, n=Son.size();    // flag 用于记录是否有结合性(是否需要合并同符号儿子的儿子为自己儿子)

    if (this->Op=='+' || this->Op=='*')
        flag = 1;
    if (n==0) // 理应不为0，但还是判一下以防万一吧
        return 0;

    for (std::unique_ptr<ExprAST> &o: Son) {
        ret |= o->simplify_LevelingOperators();    // 把儿子节点 o 缩完，更新ret
        ret |= (flag && o->get_op()==this->Op);    // 若需要提点，更新ret，具体提点在后面提
    }
    
    if (ret == 0)   // 不需要修改，则不修改
        return 0;
    
    std::vector<std::unique_ptr<ExprAST>> tmp;   // 用来存改动后的儿子，完了之后取代 Son

    for (std::unique_ptr<ExprAST> &o: Son) {
        // 以下，o 子树中有可能提为当前节点儿子的节点，只有可能是 o 或者 o 的直接儿子

        if (flag && o->get_op() == this->Op) { // o 的直接儿子提为当前节点的儿子
            for (std::unique_ptr<ExprAST> &os: *(o->get_Son()))
                tmp.push_back(std::move(os));
            // 这里之后 o 就永远用不上了
        }
        else {  // o 继续作为当前儿子的儿子
            tmp.push_back(std::move(o));
        }
    }
    
    Son = std::move(tmp);
    return ret;
}


int OpExprAST::simplify_SimplifyingRationalExpressions()
{
    int ret=0;
    if (Op=='/') {  // 除法节点理应只有且只有两个儿子
        
        // (a/b) / c = a / (c*b)
        ret |= Son[0]->simplify_SimplifyingRationalExpressions();
        if (Son[0]->get_op()=='/') {
            ret |= 1;
            Son[1]=std::make_unique<OpExprAST>('*', std::move(Son[1]), std::move((*(Son[0]->get_Son()))[1]));
            Son[0]=std::move((*(Son[0]->get_Son()))[0]);
        }
        // a / (b/c) = (a*b) / c
        ret |= Son[1]->simplify_SimplifyingRationalExpressions();
        if (Son[1]->get_op()=='/') {
            ret |= 1;
            Son[0]=std::make_unique<OpExprAST>('*', std::move(Son[0]), std::move((*(Son[1]->get_Son()))[1]));
            Son[1]=std::move((*(Son[1]->get_Son()))[0]);
        }
    }
    else if (Op=='*') {
        int n=Son.size();

        for (int i=0; i<n; i++) {
            ret |= Son[i]->simplify_SimplifyingRationalExpressions();
            // a * (b/c) = (a*b) / c
            // 处理到第一个除法，就将当前乘节点变为除节点，并重新化简
            if (Son[i]->get_op()=='/') {
                ret |= 1;
                // 新的 Son 元素暂时存到tmp，处理完了再取代之
                std::unique_ptr<ExprAST> tu, tv;
                tu = (std::move((*(Son[i]->get_Son()))[0]));
                tv = (std::move((*(Son[i]->get_Son()))[1]));
                std::vector<std::unique_ptr<ExprAST>> tmp;

                tmp.push_back(std::make_unique<OpExprAST>('*', std::move(Son[0]), std::move(Son[1])));
                for (int j = 2; j < n; j++)
                    (*(tmp[0]->get_Son()))[j] = std::move(Son[j]);
                (*(tmp[0]->get_Son()))[i] = std::move(tu);
                tmp.push_back(std::move(tv));
                
                Son=std::move(tmp);
                Op='/';

                ret |= this->simplify_SimplifyingRationalExpressions(); // 变成除节点后，再来一遍当前化简
                n=2;
                break;  // 重要!!!
            }
        }
    }
    else {
        for (std::unique_ptr<ExprAST> &o: Son)
            ret |= o->simplify_SimplifyingRationalExpressions();
    }
    return ret;
}







void OpExprAST::dfsPrint() const
{
    std::cerr <<'\n';
    std::string tmp;
    this->append_string(tmp);
    std::cerr <<"\"" <<tmp <<"\" := \n    { \'";
    std::cerr <<Lexer::op_to_string(Op) <<"\' | ";
    for (const std::unique_ptr<ExprAST> &o: Son) {
        std::string s;
        o->append_string(s);
        std::cerr <<" [ "<<s <<" ] ";
    }
    std::cerr <<" }\n\n";


    for (const std::unique_ptr<ExprAST> &o: Son) {
        o->dfsPrint();
    }
}
