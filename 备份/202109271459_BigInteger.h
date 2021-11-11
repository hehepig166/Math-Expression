#ifndef HEHEPIG_BIGINTEGER
#define HEHEPIG_BIGINTEGER

#include <iostream>
#include <vector>

class BigInteger {


public:
    BigInteger();
    BigInteger(const BigInteger &X);
    BigInteger(int x);
    ~BigInteger();

    int getlen() const;

    // 负号
	BigInteger operator-() const;

    // 加
    BigInteger& operator+=(const BigInteger &X);
    friend BigInteger operator+(const BigInteger &L, const BigInteger &R);
    BigInteger& operator++();  // ++x
    BigInteger operator++(int);  // x++

    // 减
    BigInteger& operator-=(BigInteger X);
    friend BigInteger operator-(const BigInteger &L, const BigInteger &R);
    BigInteger& operator--();
    BigInteger operator--(int);

    // 乘
    BigInteger& operator*=(const int x);
	BigInteger& operator*=(const BigInteger &X);
	friend BigInteger operator*(BigInteger L, const BigInteger &R);
	friend BigInteger operator*(const BigInteger &L, int r);
	friend BigInteger operator*(int l, const BigInteger &R);

    // 除
    friend BigInteger& operator/=(BigInteger &L, const BigInteger &R);
	friend BigInteger operator/(BigInteger L, const BigInteger &R);

    // 取模
    friend BigInteger& operator%=(BigInteger &L, const BigInteger &R);
	friend BigInteger operator%(BigInteger L, const BigInteger &R);


    // abs(L) > abs(R)
	friend bool absGreater(const BigInteger &L, const BigInteger &R);

    friend bool operator==(const BigInteger &L, const BigInteger &R);
    friend bool operator!=(const BigInteger &L, const BigInteger &R);
    friend bool operator<(const BigInteger &L, const BigInteger &R);
    friend bool operator>(const BigInteger &L, const BigInteger &R);
    friend bool operator<=(const BigInteger &L, const BigInteger &R);
    friend bool operator>=(const BigInteger &L, const BigInteger &R);


    friend std::ostream& operator<<(std::ostream &out, const BigInteger &A);

protected:

//成员变量
    static const int BIT=1000;  // 进制，理应是十的幂
	std::vector<int> a;  // 每一位上的数
	int len;  // 长度
	int sign;	// 0->零;  1->正数;  -1->负数;

//成员函数
    // 初始化为值 x（默认为零）
	void init(int x=0);

    // 检查，将前导零去掉，并将 len 置为正确值（有效位数），若为 0， sign 和 len 都置零
	void check();


	void copy(const BigInteger &X);


    // *this += X
	void plus(const BigInteger &X);

    // *this *= x (乘一个int，理应保证 BIT*(x+1) 不会溢出)
	void multiply(int x);

    // *this *= X (NTT 加速，O(n lgn))
	void multiply(const BigInteger &X);

    // A = B*Q+R， 将答案存到 Q, R 中。A 长 n，B长 m，下标从零开始（零位是最小位）。都为正数（不等于零）。
	// 若商为零，Q 将为空
	void vectorDivide(const std::vector<int> &A, const std::vector<int> &B, std::vector<int> &Q, std::vector<int> &R, int n, int m);

    // *this /= X
	void divide(const BigInteger &X);

    // *this %= X
	void modulo(const BigInteger &X);

};





#endif