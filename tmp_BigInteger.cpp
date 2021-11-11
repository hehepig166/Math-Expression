#include "./common/BigInteger.h"
#include <iostream>
#include <cstdio>
using namespace std;


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

/*
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
*/


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
		printf("%lld %d\n",k, X.get_len());
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
