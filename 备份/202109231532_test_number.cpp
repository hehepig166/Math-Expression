#include <cstdio>
#include <vector>
#include <iostream>

using namespace std;





class BigInteger {

protected:
	int BIT;  // 进制，理应是十的幂
	vector<int> a;  // 每一位上的数
	int len;  // 长度
	int sign;	// 0->零;  1->正数;  -1->负数;


	// 初始化为值 x（默认为零）
	void init(int x=0)
	{
		BIT = 1000;
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
	void check()
	{
		while (a.size() && a.back()==0)
			a.pop_back();
		
		if (a.size()==0)
			sign=len=0;
		else
			len=a.size();
	}


	// *this=X
	void copy(const BigInteger &X)
	{
		*this=X;
	}




	// *this += X
	void plus(const BigInteger &X)
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

		A.plus(B);

		for (int i=0; i<A.len; i++)
			A.a[i]=BIT-1-A.a[i];
		
		// 以上，A= abs(*this + X)
		A.sign=sig;	//加上符号，就是答案了（到这里，还存在前导零、刚好是零的情况）
		copy(A);
		check();	//去除前导零，并判断是否为零（若是则 sign 置零）
	}


	


public:
	BigInteger()
	{
		init(0);
	}

	BigInteger(const BigInteger &X)
	{
		copy(X);
	}

	BigInteger(int x)
	{
		init(x);
	}

	~BigInteger()
	{

	}


	BigInteger& operator+=(const BigInteger &X)
	{
		plus(X);
		return *this;
	}
	friend BigInteger operator+(const BigInteger &L, const BigInteger &R)
	{
		BigInteger ret(L);
		ret.plus(R);
		return ret;
	}
	BigInteger& operator++()  // ++x
	{
		plus(1);
		return *this;
	}
	BigInteger operator++(int)  // x++
	{
		BigInteger ret(*this);
		plus(1);
		return ret;
	}

	BigInteger& operator-=(BigInteger X)
	{
		if (X.sign)
			X.sign*=-1;
		plus(X);
		return (*this);
	}
	friend BigInteger operator-(const BigInteger &L, const BigInteger &R)
	{
		BigInteger ret(R);
		if (ret.sign)
			ret.sign*=-1;
		ret.plus(L);
		return ret;
	}
	BigInteger& operator--()
	{
		plus(-1);
		return *this;
	}
	BigInteger operator--(int)
	{
		BigInteger ret(*this);
		plus(-1);
		return ret;
	}




	// abs(L) > abs(R)
	friend bool absGreater(const BigInteger &L, const BigInteger &R)
	{
		if (L.len != R.len)
			return L.len > R.len;
		for (int i=L.len-1; i>=0; i--)
			if (L.a[i] != R.a[i])
				return L.a[i] > R.a[i];
		return false;
	}


	friend ostream& operator<<(ostream &out, const BigInteger &A)
	{
		if (A.sign < 0)
			out <<'-';
		if (A.sign == 0)
			out <<'0';
		else {
			//这一坨代码是处理类似万进制数中间某一位前面全是0，要把那些零都输出来
			int bitlen=1,bb=1;
			for (int k=A.BIT/10; k>1; k/=10)
				bitlen++,bb*=10;
			out <<A.a[A.len-1];
			for (int i=A.len-2; i>=0; i--) {
				for (int k=bb; k>0; k/=10)
					out <<A.a[i]/k%10;
			}
		}
		return out;
	}

	int getlen()
	{
		return len;
	}

};




void test()
{
	BigInteger a,b(666233),c(-114514),d,e;
	d+=1919810;
	cout <<a <<endl;
	cout <<b <<endl;
	cout <<c <<endl;
	cout <<d <<endl;
	c+=114510;
	cout <<c <<" (-114514 + 114510 = -4)" <<endl;
	c+=15;
	cout <<c << " (-4 + 15 = 11)" <<endl;
	c+=-11;
	cout <<c << " (11 + -11 = 0)" <<endl;

	a=0;
	cout <<a <<" (0)" <<endl;

	for (int i=1; i<=50; i++)
		a++;
	cout <<a <<" (50)" <<endl;

	for (int i=1; i<=100; i++)
		a-=i;
	cout <<a <<" (-5000)" <<endl;

	a+=5000;
	cout <<a <<" (0)" <<endl;

	e=1000000000;
	cout <<e <<" (1000000000)" <<endl;
	
}

int main()
{
	test();
	return 0;
}
