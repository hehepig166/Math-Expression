#include <iostream>
#include <string>
#include <cstring>
#include "BigInteger.h"
#include "RationalNumber.h"
using namespace std;

typedef BigInteger INT;

static const BigInteger INT_1 = BigInteger(1);
static const BigInteger INT_0 = BigInteger(0);


static const RationalNumber RAT_0 = RationalNumber(0);
static const RationalNumber RAT_1 = RationalNumber(1);



static INT gcd(INT A, INT B)
{
    return B==0 ? A : gcd(B, A%B);
}


const BigInteger RationalNumber::def_u = BigInteger(0);
const BigInteger RationalNumber::def_v = BigInteger(1);
const int RationalNumber::def_sign = 0;
const int RationalNumber::def_iinf = 0;




bool RationalNumber::is_zero() const
{
    return sign == 0;
}

bool RationalNumber::is_inf() const
{
    return iinf;
}

int RationalNumber::get_sign() const
{
    return sign;
}
BigInteger RationalNumber::get_numerator() const
{
    return u;
}
BigInteger RationalNumber::get_denominator() const
{
    return v;
}


void RationalNumber::simplify(int GCDlimit)
{
    if (u==INT_0 && v==INT_0)
    {
        cerr <<"err(RationalNumber): 0/0" <<'\n';
        init();
        return;
    }

    // 不是零的话，计算符号
    if (u!=0) {
        if (sign != 1 && sign!=-1)
            sign=1; //这是因为存在 0+x 的情况
        
        if (u<0) {
            u.reverse();
            sign*=-1;
        }
        if (v<0) {
            v.reverse();
            sign*=-1;
        }
    }


    // 零
    if (u==INT_0) {
        v = INT_1;
        sign = 0;
        iinf = 0;
        return;
    }
    // 无穷（符号保持不变）
    if (v==INT_0) {
        u = INT_1;
        iinf = 1;
        return;
    }

    iinf=0;

    if (u.get_len()>GCDlimit || v.get_len()>GCDlimit) {
        INT g=gcd(u,v);
        if (g==INT_1)
            return;
        u/=g;
        v/=g;
    }

    return;
}


void RationalNumber::init()
{
    u.copy(def_u);
    v.copy(def_v);
    sign=def_sign;
    iinf=def_iinf;
}

void RationalNumber::init(const BigInteger &u_)
{
    u.copy(u_);
    v.copy(INT_1);
    sign=0;

    //cout <<"%"<< INT_1 <<"%"<< endl;

    simplify();
}

void RationalNumber::init(const BigInteger &u_, const BigInteger &v_)
{
    u.copy(u_);
    v.copy(v_);
    sign=0;

    simplify();
}

void RationalNumber::init(const string &S, int &ptr)
{
    u.copy(INT_0);
    v.copy(INT_1);
    sign=0;

    bool flag_head=true, flag_dot=false;
    int sz=S.size(), ch;
    ptr--;

    while (++ptr<sz) {
        ch=S[ptr];
        
        if (isblank(ch) && flag_head) {
            continue;
        }
        else if (ch=='-' && flag_head) {
            v.reverse();
        }
        else if (ch=='.' && !flag_dot) {
            flag_dot=true;
        }
        else if (ch>='0' && ch<='9') {
            u*=10;
            u+=(ch-'0');
            if (flag_dot)
                v*=10;
        }
        else {
            break;
        }

        flag_head=false;
    }

    simplify();
}

void RationalNumber::copy(const RationalNumber &X)
{
    u=X.u;
    v=X.v;
    sign=X.sign;
    iinf=X.iinf;
}


RationalNumber::RationalNumber()
{
    init();
}

RationalNumber::RationalNumber(const BigInteger &U, const BigInteger &V)
{
    init(U, V);
}

RationalNumber::RationalNumber(long long X)
{
    init(INT(X));
}

RationalNumber::RationalNumber(const BigInteger &X)
{
    init(X);
}

RationalNumber::RationalNumber(const string &S)
{
    int ptr=0;
    init(S, ptr);
}

RationalNumber::RationalNumber(const string &S, int &ptr)
{
    init(S, ptr);   
}

RationalNumber::RationalNumber(const RationalNumber &X)
{
    copy(X);
}





void RationalNumber::reverse_add()
{
    sign*=-1;
}

void RationalNumber::reverse_multiply()
{
    INT tmp=u;
    u=v;
    v=tmp;
    simplify();
}




void RationalNumber::add(const RationalNumber &X)
{
    // 自己是零，为 X
    if (is_zero()) {
        copy(X);
        return;
    }
    // 加零，不变
    if (X.is_zero()) {
        return;
    }
    // 有限加无限，为无限（为 X）
    if (!is_inf() && X.is_inf()) {
        copy(X);
        return;
    }
    // 无限加有限，为无限（为自己）
    if (is_inf() && !X.is_inf()) {
        return;
    }

    // 将符号同意附着在分子 u 上面
    u = sign*u*X.v + v*X.sign*X.u;
    v*= X.v;
    sign = 1;
    // 若是一正一负两个无限，会变成 0/0， simplify() 之后置回零
    simplify();
}

void RationalNumber::multiply(const RationalNumber &X)
{
    // 自己是零，不变
    if (is_zero()) {
        return;
    }
    // 乘零，变成零
    if (X.is_zero()) {
        init(0);
        return;
    }

    // 分子、分母、符号分别直接相乘就行了
    u*=X.u;
    v*=X.v;
    sign*=X.sign;

    simplify();
}

void RationalNumber::divide(const RationalNumber &X)
{
    if (*this == X && !is_zero()) {
        init(1);
        return;
    }
    u*=X.v;
    v*=X.u;
    if (!X.is_zero())
        sign*=X.sign;   // 为了让类似 a/0 的时候，能得出符号正确的 inf
    
    simplify();
}









RationalNumber RationalNumber::operator-() const
{
    RationalNumber ret(*this);
    ret.reverse_add();
    return ret;
}



RationalNumber& RationalNumber::operator+=(const RationalNumber &X)
{
    add(X);
    return *this;
}
RationalNumber operator+(const RationalNumber &L, const RationalNumber &R)
{
    if (L.is_zero())
        return R;
    if (R.is_zero())
        return L;
    return RationalNumber(L)+=R;
}




RationalNumber& RationalNumber::operator-=(const RationalNumber &X)
{
    reverse_add();
    add(X);
    reverse_add();
    return *this;
}
RationalNumber operator-(const RationalNumber &L, const RationalNumber &R)
{
    if (R.is_zero())
        return L;
    return RationalNumber(L)-=R;
}



RationalNumber& RationalNumber::operator*=(const RationalNumber &X)
{
    multiply(X);
    return *this;
}
RationalNumber operator*(const RationalNumber &L, const RationalNumber &R)
{
    if (L.is_zero() || R.is_zero())
        return RAT_0;
    RationalNumber ret(L);
    ret.multiply(R);
    return ret;
}


RationalNumber& RationalNumber::operator/=(const RationalNumber &X)
{
    divide(X);
    return *this;
}
RationalNumber operator/(const RationalNumber &L, const RationalNumber &R)
{
    RationalNumber ret(L);
    ret.divide(R);
    return ret;
}


BigInteger operator%(const RationalNumber &L, const RationalNumber &R)
{
    RationalNumber l(L), r(R);
    l.simplify();
    r.simplify();
    if (l.get_denominator()!=INT_1 || r.get_denominator()!=INT_1) {
        cerr <<"err(RationalNumber) fraction opr %" <<endl;
        return INT_0;
    }
    BigInteger ret(L.u);
    ret%=R.u;
    if (L.sign*R.sign<0)
        ret.reverse();
    return ret;
}







bool RationalNumber::absGreater(const RationalNumber &L, const RationalNumber &R)
{
    if (L.sign != R.sign)
        return L.sign > R.sign;
    
    if (L.sign == 0)
        return false;
    
    return L.sign*L.u*R.v > R.sign*R.u*L.v;
}



bool operator==(const RationalNumber &L, const RationalNumber &R)
{
    if (L.sign != R.sign)
        return false;
    if (L.iinf != R.iinf)
        return false;
    
    return L.sign*L.u*R.v == R.sign*R.u*L.v;
}
bool operator!=(const RationalNumber &L, const RationalNumber &R)
{
    return !(L==R);
}


bool operator<(const RationalNumber &L, const RationalNumber &R)
{
    if (L.sign != R.sign)
        return L.sign < R.sign;
    if (L.sign == 0)
        return false;
    
    return (L.sign < 0) ? RationalNumber::absGreater(L, R) : RationalNumber::absGreater(R, L);
}
bool operator>(const RationalNumber &L, const RationalNumber &R)
{
    if (L.sign != R.sign)
        return L.sign > R.sign;
    if (L.sign == 0)
        return false;
    
    return (L.sign > 0) ? RationalNumber::absGreater(L, R) : RationalNumber::absGreater(R, L);
}

bool operator<=(const RationalNumber &L, const RationalNumber &R)
{
    return !(L>R);
}
bool operator>=(const RationalNumber &L, const RationalNumber &R)
{
    return !(L<R);
}





void RationalNumber::append_string(std::string &Str) const
{
    if (this->is_zero()) {
        Str += '0';
        return;
    }

    if (this->sign==-1)
        Str += '-';

    if (this->is_inf()) {
        Str += "inf";
        return;
    }

    (this->u).append_string(Str);

    if (this->v!=INT_1) {
        Str += '/';
        (this->v).append_string(Str);
    }
}


std::string RationalNumber::to_string() const
{
    std::string ret;
    this->append_string(ret);
    return ret;
}


void RationalNumber::print(ostream& out, int mode) const
{
    RationalNumber tmp = (*this);

    if (mode==-1) {
        tmp.simplify(0);    // 因为要输出了，所以必须要约分
        out <<'{' <<tmp.sign <<" (" <<tmp.u <<'/' <<tmp.v <<") " <<tmp.iinf <<'}';
        return;
    }

    if (tmp.is_zero()) {
        out <<'0';
        return;
    }

    if (tmp.sign==-1)
        out <<'-';

    if (tmp.is_inf()) {
        out <<"inf";
        return;
    }

    out << tmp.u;

    if (tmp.v!=INT_1) {
        out <<'/' << tmp.v;
    }
}


ostream& operator<<(ostream &out, const RationalNumber &X)
{
    out <<X.to_string();
    return out;
}