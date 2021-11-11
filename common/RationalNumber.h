#ifndef HEHEPIG_RATIONALNUMBER
#define HEHEPIG_RATIONALNUMBER

#include <iostream>
#include <string>
#include "BigInteger.h"


class RationalNumber {

    static const BigInteger def_u;
    static const BigInteger def_v;
    static const int def_sign;
    static const int def_iinf;


public:
    RationalNumber();   // 缺省值为 init() 中的缺省值
    RationalNumber(const BigInteger &U, const BigInteger &V);
    RationalNumber(long long X);
    RationalNumber(const BigInteger &X);
    RationalNumber(const RationalNumber &X);
    RationalNumber(const std::string &S);
    RationalNumber(const std::string &S, int &ptr);

    bool is_inf() const;
    bool is_zero() const;
    int get_sign() const;
    BigInteger get_numerator() const;  // 分子
    BigInteger get_denominator() const;  // 分母


    // 除了无参那个，其它的 init(...) 里面调用了 simplify()
    void init();
    void init(const BigInteger &u_);
    void init(const BigInteger &u_, const BigInteger &v_);
    void init(const std::string &S, int &ptr);
    
    // copy() 里面没有调用 simplify()
    void copy(const RationalNumber &X);

    // 检查，若出现 0/0，输出错误提示，并置缺省值
    // 当长度超过 GCDlimit 时才约分，否则只是进行是否为零、无穷等的检查
    // 在输出函数中，理应进行一次 simplify(0) 以约分
    void simplify(int GCDlimit=0);

    // 自取逆元
    void reverse_add();
    void reverse_multiply();

    RationalNumber operator-() const;

    RationalNumber& operator+=(const RationalNumber &X);
    friend RationalNumber operator+(const RationalNumber &L, const RationalNumber &R);

    RationalNumber& operator-=(const RationalNumber &X);
    friend RationalNumber operator-(const RationalNumber &L, const RationalNumber &R);

    RationalNumber& operator*=(const RationalNumber &X);
    friend RationalNumber operator*(const RationalNumber &L, const RationalNumber &R);

    RationalNumber& operator/=(const RationalNumber &X);
    friend RationalNumber operator/(const RationalNumber &L, const RationalNumber &R);

    friend BigInteger operator%(const RationalNumber &L, const RationalNumber &R);



    static bool absGreater(const RationalNumber &X, const RationalNumber&R);

    friend bool operator==(const RationalNumber &L, const RationalNumber &R);
    friend bool operator!=(const RationalNumber &L, const RationalNumber &R);
    friend bool operator<(const RationalNumber &L, const RationalNumber &R);
    friend bool operator>(const RationalNumber &L, const RationalNumber &R);
    friend bool operator<=(const RationalNumber &L, const RationalNumber &R);
    friend bool operator>=(const RationalNumber &L, const RationalNumber &R);



    void append_string(std::string &Str) const;
    std::string to_string() const;

    void print(std::ostream& out =std::cout, int mode =0) const;

    friend std::ostream& operator<<(std::ostream &in, const RationalNumber &X);

protected:
//成员变量
    BigInteger u;  // 分子，理应为自然数
    BigInteger v;  // 分母，理应为自然数
    int sign;   // 0: 零 -1:负数 1:正数，可信值只有这三个
    bool iinf;  // is_inf

//成员函数
    // 因为加法逆元可以很快取得，不需要额外的复制空间，所以就不弄减法了
    void add(const RationalNumber &X);
    void multiply(const RationalNumber &X);
    void divide(const RationalNumber &X);

};











#endif