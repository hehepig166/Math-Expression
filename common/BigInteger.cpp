#include "BigInteger.h"
#include <cstdio>
#include <iostream>
#include <vector>
using namespace std;

const int BigInteger::BIT=1000;




namespace NTT {
	static const long long P=998244353;	// =119*2^23+1
	static const long long G=3;
	static vector<int> r;

	static long long qpow(long long x, int k)
	{
		int ret=1;
		for (; k; x=x*x%P, k>>=1)
			if (k&1)
				ret=ret*x%P;
		return ret;
	}

	static void ntt(vector<long long> &X, int lim, int opt)
	{
		register long long gn,g,tmp;
		for (int i=0; i<lim; i++)
			if (r[i]<i)
				swap(X[i], X[r[i]]);
		for (int m=2, k=1; m<=lim; m<<=1, k<<=1) {
			gn=qpow(G, (P-1)/m);
			for (int i=0; i<lim; i+=m) {
				g=1;
				for (int j=0; j<k; j++, g=g*gn%P) {
					tmp= X[i+j+k]*g %P;
					X[i+j+k]= (X[i+j]-tmp+P)%P;
					X[i+j]=(X[i+j]+tmp)%P;
				}
			}
		}
		if (opt==-1) {
			for (int i=1,j=lim-1; i<j; i++,j--)
				swap(X[i],X[j]);
			long long inv=qpow(lim, P-2);
			for (int i=0; i<lim; i++)
				X[i]=X[i]*inv%P;
		}
	}

	static void multiply(const vector<int> &A_, const vector<int> &B_, vector<long long> &C, int lenA, int lenB)
	{
		int len=lenA+lenB+1, lim;
		for (lim=1; lim < len; lim<<=1)
			continue;

		r.resize(lim);
		for (int i=0; i<lim; i++)
			r[i]= (i&1)*(lim>>1)+(r[i>>1]>>1);
		
		vector<long long> A(lim), B(lim);
		for (int i=0; i<lenA; i++)
			A[i]=A_[i];;
		for (int i=0; i<lenB; i++)
			B[i]=B_[i];

		ntt(A, lim, 1);
		ntt(B, lim, 1);

		C.resize(lim);
		for (int i=0; i<lim; i++)
			C[i]=A[i]*B[i] %P;
		ntt(C, lim, -1);
	}
}




// 初始化为默认值 0
void BigInteger::init()
{
	a.clear();
	len = 0;
	sign=0;
}

// 初始化为值 x（默认为零）
void BigInteger::init(long long x)
{
	a.clear();
	len = 0;

	if (x == 0) {
		sign = 0;
		return;
	}

	if (x < 0) {
		x = -x;
		sign = -1;
	}
	else {
		sign = 1;
	}

    while (x > 0) {
		a.push_back(x % BIT);
		len++;
		x /= BIT;
	}
}





// 检查，将前导零去掉，并将 len 置为正确值（有效位数），若为 0， sign 和 len 都置零
void BigInteger::check()
{
	while (a.size() && a.back()==0)
		a.pop_back();
		
	if (a.size()==0)
		sign=len=0;
	else
		len=a.size();
}


// *this=X
void BigInteger::copy(const BigInteger &X)
{
	*this=X;
}



void BigInteger::reverse()
{
	sign*=-1;
}



int BigInteger::get_len() const
{
	return len;
}
int BigInteger::get_sign() const
{
	return sign;
}



// *this += X
void BigInteger::add(const BigInteger &X)
{
	// 加零，不变
	if (X.sign==0) {
		return;
	}

	// 自己是零，复制
	if (this->sign==0) {
		copy(X);
		return;
	}

	// 同号，绝对值相加
	if (this->sign==X.sign) {
		int c=0;  //进位
		for (int i=0; i<len; i++) {
			a[i]+=c;
			if (i<X.len)
				a[i]+=X.a[i];

			c=a[i]/BIT;
			a[i]%=BIT;
		}
		for (int i=len; i<X.len; i++) {
			c+=X.a[i];
			a.push_back(c%BIT);
			len++;
			c/=BIT;
		}
		if (c>0) {
			a.push_back(c);
			len++;
		}
		return;
	}
		

	// 异号(都不为零)
	/*
	假设 A > B > 0 (为了保证 A'+B < A'+A)
	取补： a'=BIT-1-a
	令 A' 为 A 每一位都取补后的值(即 A+A' = 999..9 = (BIT^(A.len)-1))
	-A + B = A' + B - (A+A')
	       = (A' + B)'
	*/
	BigInteger A,B;
	int sig;
	if (absGreater(*this, X)) {
		A.copy(*this);
		B.copy(X);
		A.sign=B.sign=1;  //注意这里是因为前面确保了 AB 都不为零，所以可以赋值 1，这样是为了后面加法运算可以直接调用本函数（用上面的同号情况）
		sig=this->sign;  //答案的符号为绝对值更大的那个的符号（若为零，会在最后的check()中处理掉）
	}
	else {
		A.copy(X);
		B.copy(*this);
		A.sign=B.sign=1;
		sig=X.sign;
	}

	for (int i=0; i<A.len; i++)  // A= A'
		A.a[i]=BIT-1-A.a[i];
	A.check();  // 去除前导零

	A.add(B);

	for (int i=0; i<A.len; i++)
		A.a[i]=BIT-1-A.a[i];
		
	// 以上，A= abs(*this + X)
	A.sign=sig;	//加上符号，就是答案了（到这里，还存在前导零、刚好是零的情况）
	copy(A);
	check();	//去除前导零，并判断是否为零（若是则 sign 置零）
}


// *this *= x (乘一个int，理应保证 BIT*(x+1) 不会溢出)
void BigInteger::multiply(int x)
{
	if (sign==0) {
		return;
	}

	if (x==0) {
		init(0);
		return;
	}

	if (x<0) {
		sign*=-1;
		x=-x;
	}

	if (x==1) {
		return;
	}

	int c=0;
	for (int i=0; i<len; i++) {
		a[i]=a[i]*x+c;
		c=a[i]/BIT;
		a[i]%=BIT;
	}
	while (c>0) {
		a.push_back(c%BIT);
		len++;
		c/=BIT;
	}
}


// *this *= X (NTT 加速，O(n lgn))
void BigInteger::multiply(const BigInteger &X)
{
	// 自己是零，不变
	if (sign==0) {
		return;
	}
	// 乘零，直接置零
	if (X.sign==0) {
		init(0);
		return;
	}

	sign*=X.sign;  //先处理符号，剩下就是处理绝对值了

	// 乘一，绝对值不变
	if (X.len==1 && X.a[0]==1) {
		return;
	}

	// 其他情况，用 NTT 算绝对值
	vector<long long> C;
	NTT::multiply(a, X.a, C, len, X.len);

	// 整理一下 C，确保每一位小于 BIT
	long long cnt=C.size(), ccc=0;
	for (int i=0; i<cnt; i++) {
		C[i]+=ccc;
		ccc=C[i]/BIT;
		C[i]%=BIT;
	}
	while (ccc) {
		C.push_back(ccc%BIT);
		ccc/=BIT;
	}

	// 把答案转移到自己的系数数组上
	len=C.size();
	a.resize(len);
	for (int i=0; i<len; i++)
		a[i]=(int)C[i];

	// 最后check一下，把前导零去掉
	check();
}


// A = B*Q+R， 将答案存到 Q, R 中。A 长 n，B长 m，下标从零开始（零位是最小位）。都为正数（不等于零）。
// 若商为零，Q 将为空
void BigInteger::vectorDivide(const vector<int> &A, const vector<int> &B, vector<int> &Q, vector<int> &R, int n, int m)
{
	R=A;
	if (n<m) {
		Q.clear();
		return;
	}

	vector<int> tmp(m+1);
	Q.resize(n-m+1);
	R.push_back(0);
	for (int i=n-m; i>=0; i--) {
		Q[i]=0;
		int ll=0, rr=(R[i+m]*BIT+R[i+m-1])/B[m-1], M;

		while (ll<=rr) {
			M=(ll+rr)>>1;
			tmp[m]=0;
			for (int j=0; j<m; j++) {
				tmp[j]=B[j]*M+tmp[m];
				tmp[m]=tmp[j]/BIT;
				tmp[j]%=BIT;
			}

			int fail=0;
			for (int j=m; j>=0; j--) {
    			if (tmp[j]<R[i+j]) {
					break;
				}
				else if (tmp[j]>R[i+j]) {
					fail=1;
					rr=M-1;
					break;
				}
			}

			if (!fail) {
    			Q[i]=M;
				ll=M+1;
			}
		}

		tmp[m]=0;
		for (int j=0; j<m; j++) {
			tmp[j]=B[j]*Q[i]+tmp[m];
			tmp[m]=tmp[j]/BIT;
			tmp[j]%=BIT;
		}
		for (int j=m; j>=0; j--) {
			if ((R[i+j]-=tmp[j])<0) {
				R[i+j+1]--;
				R[i+j]+=BIT;
			}
		}
	}
    while (Q.size() && Q.back()==0)
		Q.pop_back();
	while (R.size() && R.back()==0)
		R.pop_back();
}




// *this /= X
void BigInteger::divide(const BigInteger &X)
{
	// 除以零，出错
	if (X.sign==0) {
		cerr <<"err(BigInteger): divided by zero" <<endl;
		return;
	}
	// 自己是零，不变
	if (sign==0) {
		return;
	}
	// 自己较小，答案为零
	if (absGreater(X, *this)) {
		init(0);
		return;
	}
	// 相等，答案为一
	if (*this == X) {
		init(1);
		return;
	}
	// 除以一，不变
	if (X == 1) {
		return;
	}

	vector<int> ret;
	vectorDivide(a, X.a, ret, a, len, X.len);

	a=ret;
	sign*=X.sign;	// 符号与除数与被除数的符号相乘所得相同
	check();
}

// *this %= X
void BigInteger::modulo(const BigInteger &X)
{
	// 除以零，出错
	if (X.sign==0) {
		cout <<"err: divided by zero" <<endl;
		return;
	}
	// 自己是零，不变
	if (sign==0) {
		return;
	}
	// 自己较小，不变
	if (absGreater(X, *this)) {
		return;
	}
	// 相等，答案为零
	if (*this == X) {
		init(0);
		return;
	}
	// 模一，答案为零
	if (X == 1) {
		init(0);
		return;
	}

	vector<int> ret;
	vectorDivide(a, X.a, ret, a, len, X.len);

	// 余的符号为被除数的符号
	check();
}









BigInteger::BigInteger()
{
	init(0);
}

BigInteger::BigInteger(const BigInteger &X)
{
	copy(X);
}

BigInteger::BigInteger(long long x)
{
	init(x);
}

// 负号
BigInteger BigInteger::operator-() const
{
	BigInteger ret;
	ret.copy(*this);
	ret.sign*=-1;
	return ret;
}


BigInteger& BigInteger::operator+=(const BigInteger &X)
{
	add(X);
	return *this;
}
BigInteger operator+(const BigInteger &L, const BigInteger &R)
{
	BigInteger ret(L);
	ret.add(R);
	return ret;
}
BigInteger& BigInteger::operator++()  // ++x
{
	add(1);
    return *this;
}
BigInteger BigInteger::operator++(int)  // x++
{
	BigInteger ret(*this);
	add(1);
	return ret;
}

BigInteger& BigInteger::operator-=(BigInteger X)
{
	if (X.sign)
		X.sign*=-1;
	add(X);
	return (*this);
}
BigInteger operator-(const BigInteger &L, const BigInteger &R)
{
	BigInteger ret(R);
	if (ret.sign)
		ret.sign*=-1;
	ret.add(L);
	return ret;
}
BigInteger& BigInteger::operator--()
{
	add(-1);
	return *this;
}
BigInteger BigInteger::operator--(int)
{
	BigInteger ret(*this);
	add(-1);
	return ret;
}

BigInteger& BigInteger::operator*=(const int x)
{
	multiply(x);
	return *this;
}
BigInteger& BigInteger::operator*=(const BigInteger &X)
{
	multiply(X);
	return *this;
}
BigInteger operator*(BigInteger L, const BigInteger &R)
{
	return (L*=R);
}
BigInteger operator*(const BigInteger &L, int r)
{
	BigInteger ret(r);
	return (ret*=L);
}
BigInteger operator*(int l, const BigInteger &R)
{
	BigInteger ret(l);
	return (ret*=R);
}

BigInteger& operator/=(BigInteger &L, const BigInteger &R)
{
	L.divide(R);
	return L;
}
BigInteger operator/(BigInteger L, const BigInteger &R)
{
	L.divide(R);
	return L;
}

BigInteger& operator%=(BigInteger &L, const BigInteger &R)
{
	L.modulo(R);
	return L;
}
BigInteger operator%(BigInteger L, const BigInteger &R)
{
	L.modulo(R);
	return L;
}



// abs(L) > abs(R)
bool BigInteger::absGreater(const BigInteger &L, const BigInteger &R)
{
	if (L.len != R.len)
		return L.len > R.len;
	for (int i=L.len-1; i>=0; i--)
		if (L.a[i] != R.a[i])
			return L.a[i] > R.a[i];
	return false;
}


bool operator==(const BigInteger &L, const BigInteger &R)
{
	if (L.len != R.len)
		return false;
	if (L.sign != R.sign)
		return false;
	for (int i=L.len-1; i>=0; i--)
		if (L.a[i]!=R.a[i])
			return false;
	
	return true;
}
bool operator!=(const BigInteger &L, const BigInteger &R)
{
	return !(L==R);
}

bool operator<(const BigInteger &L, const BigInteger &R)
{
	if (L.sign != R.sign)
		return L.sign < R.sign;
	
	if (L.sign == 0)
		return false;
	
	return (L.sign < 0) ? BigInteger::absGreater(L, R) : BigInteger::absGreater(R, L);
}
bool operator>(const BigInteger &L, const BigInteger &R)
{
	if (L.sign != R.sign)
		return L.sign > R.sign;
		
	if (L.sign == 0)
		return false;
		
	return (L.sign > 0) ? BigInteger::absGreater(L, R) : BigInteger::absGreater(R, L);
}

bool operator<=(const BigInteger &L, const BigInteger &R)
{
	return !(L>R);
}
bool operator>=(const BigInteger &L, const BigInteger &R)
{
	return !(L<R);
}






void BigInteger::append_string(std::string &Str) const
{
	if (this->sign < 0)
		Str += '-';
	if (this->sign == 0)
		Str += '0';
	else {
		//这一坨代码是处理类似万进制数中间某一位前面全是0，要把那些零都输出来
		int bitlen=1,bb=1;
		for (int k=this->BIT/10; k>1; k/=10)
			bitlen++,bb*=10;
		Str += std::to_string(this->a[this->len-1]);
		for (int i=this->len-2; i>=0; i--) {
			for (int k=bb; k>0; k/=10)
				Str += std::to_string(this->a[i]/k%10);
		}
	}
}

std::string BigInteger::to_string() const
{
	std::string ret;
	this->append_string(ret);
	return ret;
}




ostream& operator<<(ostream &out, const BigInteger &A)
{
	out << A.to_string();
	return out;
}

