#include <cstdio>
#include <vector>
#include <iostream>
using namespace std;







namespace NTT {
	const long long P=998244353;	// =119*2^23+1
	const long long G=3;
	vector<int> r;

	long long qpow(long long x, int k)
	{
		int ret=1;
		for (; k; x=x*x%P, k>>=1)
			if (k&1)
				ret=ret*x%P;
		return ret;
	}

	void ntt(vector<long long> &X, int lim, int opt)
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

	void multiply(const vector<int> &A_, const vector<int> &B_, vector<long long> &C, int lenA, int lenB)
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









class BigInteger {

protected:
	static const int BIT=1000;  // 进制，理应是十的幂
	vector<int> a;  // 每一位上的数
	int len;  // 长度
	int sign;	// 0->零;  1->正数;  -1->负数;


	// 初始化为值 x（默认为零）
	void init(int x=0)
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


	// *this *= x (乘一个int，理应保证 BIT*(x+1) 不会溢出)
	void multiply(int x)
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
	void multiply(const BigInteger &X)
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
			a[i]=C[i];

		// 最后check一下，把前导零去掉
		check();
	}


	// A = B*Q+R， 将答案存到 Q, R 中。A 长 n，B长 m，下标从零开始（零位是最小位）。都为正数（不等于零）。
	// 若商为零，Q 将为空
	void vectorDivide(const vector<int> &A, const vector<int> &B, vector<int> &Q, vector<int> &R, int n, int m)
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
		while (Q.back()==0)
			Q.pop_back();
		while (R.back()==0)
			R.pop_back();
	}




	// *this /= X
	void divide(const BigInteger &X)
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
		// 自己较小，答案为零
		if (absGreater(X, *this)) {
			init(0);
			return;
		}
		//相等，答案为一
		if (*this == X) {
			init(1);
			return;
		}

		vector<int> ret;
		vectorDivide(a, X.a, ret, a, len, X.len);

		a=ret;
		sign*=X.sign;	// 符号与除数与被除数的符号相乘所得相同
		check();
	}

	// *this %= X
	void modulo(const BigInteger &X)
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
		//相等，答案为零
		if (*this == X) {
			init(0);
			return;
		}

		vector<int> ret;
		vectorDivide(a, X.a, ret, a, len, X.len);

		// 余的符号为被除数的符号
		check();
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

	// 负号
	BigInteger operator-() const
	{
		BigInteger ret;
		ret.copy(*this);
		ret.sign*=-1;
		return ret;
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

	BigInteger& operator*=(const int x)
	{
		multiply(x);
		return *this;
	}
	BigInteger& operator*=(const BigInteger &X)
	{
		multiply(X);
		return *this;
	}
	friend BigInteger operator*(BigInteger L, const BigInteger &R)
	{
		return (L*=R);
	}
	friend BigInteger operator*(const BigInteger &L, int r)
	{
		BigInteger ret(r);
		return (ret*=L);
	}
	friend BigInteger operator*(int l, const BigInteger &R)
	{
		BigInteger ret(l);
		return (ret*=R);
	}

	friend BigInteger& operator/=(BigInteger &L, const BigInteger &R)
	{
		L.divide(R);
		return L;
	}
	friend BigInteger operator/(BigInteger L, const BigInteger &R)
	{
		L.divide(R);
		return L;
	}

	friend BigInteger& operator%=(BigInteger &L, const BigInteger &R)
	{
		L.modulo(R);
		return L;
	}
	friend BigInteger operator%(BigInteger L, const BigInteger &R)
	{
		L.modulo(R);
		return L;
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


	friend bool operator==(const BigInteger &L, const BigInteger &R)
	{
		if (L.len != R.len)
			return false;
		if (L.sign != R.sign)
			return false;
		if (L.a != R.a)
			return false;
		
		return true;
	}
	friend bool operator!=(const BigInteger &L, const BigInteger &R)
	{
		return !(L==R);
	}

	friend bool operator<(const BigInteger &L, const BigInteger &R)
	{
		if (L.sign != R.sign)
			return L.sign < R.sign;
		
		if (L.sign == 0)
			return false;
		
		return (L.sign < 0) ? absGreater(L, R) : absGreater(R, L);
	}
	friend bool operator>(const BigInteger &L, const BigInteger &R)
	{
		if (L.sign != R.sign)
			return L.sign > R.sign;
		
		if (L.sign == 0)
			return false;
		
		return (L.sign > 0) ? absGreater(L, R) : absGreater(R, L);
	}

	friend bool operator<=(const BigInteger &L, const BigInteger &R)
	{
		return !(L>R);
	}
	friend bool operator>=(const BigInteger &L, const BigInteger &R)
	{
		return !(L<R);
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

	int getlen() const
	{
		return len;
	}

};




















void test1()
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

	e=-e;
	cout <<e <<" (-1000000000)" <<endl;

	e-=e;
	cout <<e <<" (0)" <<endl;

	puts("\ntest1 done\n");
}


void test2()
{
	BigInteger a=1, b=666233, c=-1919810, d=0;
	
	cout << (a*=666233) <<" (666233)" <<endl;
	cout << (a*=-1919810) <<" (-1279040775730)" <<endl;
	cout << (a*=0) <<" (0)" <<endl;
	a=1;
	
	cout <<a <<" (1)" <<endl;
	cout <<(a*=b) <<" (666233)" <<endl;
	cout <<(a*=-1919810) <<" (-1279040775730)" <<endl;
	cout <<(a*=-2) <<" (2558081551460)" <<endl;
	a=5000000;
	cout <<(a*=2) <<" (10000000)" <<endl;
	cout <<(a*=d) <<" (0)" <<endl;
	cout <<(a*=b) <<" (0)" <<endl;
	cout <<(a*=a) <<" (0)" <<endl;

	puts("\ntest2 done\n");
}


void testNTT()
{
	int tmp,n=0,m=0;
	vector<int> A, B;
	vector<long long> C;
	printf("输入多项式 A 的系数，以 * 结束输入\n >");
	while (1) {
		if (scanf("%d",&tmp)!=1) {
			getchar();
			if (n)
				break;
		}
		A.push_back(tmp);
		n++;
	}
	printf("输入多项式 B 的系数，以 * 结束输入\n >");
	while (1) {
		if (scanf("%d",&tmp)!=1) {
			getchar();
			if (m)
				break;
		}
		B.push_back(tmp);
		m++;
	}

	NTT::multiply(A, B, C, n, m);

	puts("");
	printf("\nA   = { ");
	for (int i=0; i<n; i++) printf("%d ",A[i]);
	printf("}\n");
	printf("\nB   = { ");
	for (int i=0; i<m; i++) printf("%d ",B[i]);
	printf("}\n");

	printf("\nA*B = {");
	for (int i=0, mi=C.size(); i<mi; i++) printf("%lld ", C[i]);
	printf("}\n");
	
}

void test3()
{
	int a=-114514, b=-66233, c=1919810;
	BigInteger A=-114514, B=-66233, C=1919810;
	a/=b;	A/=B;
	cout <<"#Case 1.1: " <<A <<' ' <<a <<endl;
	c/=b;	C/=B;
	cout <<"#Case 1.2: " <<C <<' ' <<c <<endl;
	cout <<(A/=0) <<endl;

	a=-114514, b=-66233, c=1919810;
	A=-114514, B=-66233, C=1919810;
	a%=b; A%=B;
	cout <<"Case 2.1: " <<A <<' ' <<a <<endl;
	c%=b; C%=B;
	cout <<"Case 2.2: " <<C <<' ' <<c <<endl;

	a=-5, b=3;
	A=-5, B=3;
	a=a%b;
	A=A%B;
	cout <<"Case 2.3: " <<A <<' ' <<a <<endl;

	a=-5, b=-3;
	A=-5, B=-3;
	a=a%b;
	A=A%B;
	cout <<"Case 2.4: " <<A <<' ' <<a <<endl;

	puts("\ntest3 done\n");
}

void test4()
{
	int a,b,c,d,e,f,g;
	BigInteger A,B,C,D,E,F,G;
	a=-10; A=-10;
	b=a/2; B=A/2;
	c=-1; C=-1;
	d=0; D=0;
	e=-c; E=-C;
	f=-b; F=-B;
	g=-a+1; G=-A+1;

	cout <<"1: " << (A<B) <<' ' <<(a<b) <<endl;
	cout <<"2: " << (B<=C) <<' ' <<(b<=c) <<endl;
	cout <<"3: " << (C==C) <<' ' <<(1) <<endl;
	cout <<"4: " << (C!=E) <<' ' <<(c!=e) <<endl;
	cout <<"5: " << (D>=C) <<' ' <<(d>=c) <<endl;
	cout <<"6: " << (E>F) <<' ' <<(e>f) <<endl;
	cout <<"7: " << (G>-A) <<' ' <<(g>-a) <<endl;
	cout <<"8: " << (G<-A) <<' ' <<(g<-a) <<endl;

	puts("\ntest4 done\n");

}


void test_fabonacci()
{
	BigInteger a=1, b=1, c=1;
	int n;
	printf("Input n (Fabonacci[3..n]) > ");
	scanf("%d",&n);
	for (int i=3; i<=n; i++) {
		a=b;
		b=c;
		c=a+b;
		cout <<'[' <<i <<"]\t" <<c <<endl;
	}

	puts("\ntest_fabonacci done\n");
}

void test_pow()
{
	long long x, k;
	printf("Input x k (x^k) > ");
	scanf("%lld%lld",&x, &k);
	BigInteger ret=1, X=x;

	const BigInteger Ha=998244353;

	for (; k; X*=X, k>>=1) {
		printf("%lld %d\n",k, X.getlen());
		if (k&1)
			ret*=X;
	}	
	cout <<ret <<endl;

	puts("\ntest_pow done\n");
}

void test_pow_mod()
{
	long long x, k;
	printf("Input x k (x^k) > ");
	scanf("%lld%lld",&x, &k);
	BigInteger ret=1, X=x;

	const BigInteger Ha=998244353;

	for (; k; X*=X %Ha, k>>=1) {
		if (k&1)
			ret=X*ret %Ha;
	}	
	cout <<ret <<endl;

	puts("\ntest_pow_mod done\n");
}

int main()
{
	test1();	// 加减
	test2();	// 乘
	//testNTT();  // 测试多项式乘法
	test3();	// 除模
	test4();  // 比较
	test_fabonacci();	// 斐波那契数列
	test_pow_mod();  // 幂运算（答案取模）
	test_pow();  // 幂运算
	return 0;
}
