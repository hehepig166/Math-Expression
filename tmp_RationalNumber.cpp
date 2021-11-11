#include "./common/RationalNumber.h"
#include "./common/MyAlgorithm.h"
#include <iostream>
#include <string>
#include <cstring>
using namespace std;



void test1()
{
    RationalNumber A, B(5), C(-3), D(-6,4), E, F(9,7);
    cout <<"A \t" <<A <<endl;
    cout <<"B \t" <<B <<endl;
    cout <<"C \t" <<C <<endl;
    cout <<"D \t" <<D <<endl;
    cout <<"-B \t" <<(-B) <<endl;
    cout <<"-D \t" <<(-D) <<endl;
    cout <<"E+=B \t" <<(E+=B) <<" (5)"<<endl;
    cout <<"E+=C \t" <<(E+=C) <<" (2)"<<endl;

    puts("\n**");
    cout <<"E-=D \t" <<(E-=D) <<" (7/2)"<<endl;
    puts("**\n");
    cout <<"E+E \t" <<(E+E) <<" (7)" <<endl; 
    cout <<"F \t" <<F <<endl;
    cout <<"A-F \t" <<(A-F) <<" (-9/7)" <<endl;
    cout <<"B-F \t" <<(B-F) <<" (26/7)"<<endl;
    cout <<"C+F \t" <<(C+F) <<" (-12/7)"<<endl;
    cout <<"D-F \t" <<(D-F) <<" (-39/14)" <<endl;
    cout <<"F-D \t" <<(F-D) <<" (39/14)" <<endl;
    cout <<"D+F \t" <<(D+F) <<" (-3/14)" <<endl;
    cout <<"F+D \t" <<(F+D) <<" (-3/14)" <<endl;
    cout <<"F-F \t" <<(F-F) <<endl;
    cout <<"F+F \t" <<(F+F) <<endl;
    
    puts("\n### test1 done\n");
}


void test2()
{
    RationalNumber A, B(5), C(7,9), D(-3), E(1), F;
    cout <<"A \t" <<A <<endl;
    cout <<"B \t" <<B <<endl;
    cout <<"C \t" <<C <<endl;
    cout <<"D \t" <<D <<endl;
    cout <<"E \t" <<E <<endl;
    cout <<"E*=C \t" <<(E*=C) <<" (7/9)" <<endl;
    puts("\n**");
    cout <<"E*=D \t" <<(E*=D) <<" (-7/3)" <<endl;
    puts("**\n");
    cout <<"E*=A \t" <<(E*=A) <<" (0)" <<endl;
    cout <<"B*C \t" <<(B*C) <<" (35/9)" <<endl;
    cout <<"C*27 \t" <<(C*27) <<" (21)" <<endl;
    cout <<"-27*C \t" <<(-27*C) <<" (-21)" <<endl;
    cout <<(27*C) <<endl;

    puts("\n### test2 done\n");
}


void test3()
{
    RationalNumber A, B(1), C(5,3), D(8,-10), E(0, -9), F;
    cout <<(C/D) <<"\t (-25/12)" <<endl;
    cout <<(A/B) <<"\t (0)" <<endl;
    cout <<(B/A) <<"\t (inf)"<<endl;
    cout <<(-B/A) <<"\t (-inf)"<<endl;
    cout <<(A/A) <<endl;
}

void test4()
{
    RationalNumber A, B, C;
    int ptr=0;
    char opr;
    string S;

    cout <<endl <<"A opr B (space is not necessary, 'h' for help)> ";
    getline(cin, S);

    A.init(S, ptr);
    while (isblank(S[ptr]))
        ptr++;
    opr=S[ptr++];
    B.init(S, ptr);

    switch(opr) {
        case '+':
            C=A+B;
            break;
        case '-':
            C=A-B;
            break;
        case '*':
            C=A*B;
            break;
        case '/':
            C=A/B;
            break;
        case '%':
            C=A%B;
            break;
        case '^':
            C=my_algorithm::qpow(A,B.get_numerator());
            break;

        case 'h':
            cout <<"Rational number: 1 -1 .5 -0.5" <<endl;
            cout <<"opr: + - * / % ^ < = >" <<endl;
            cout <<"samples: -2^1000000 0.123456789123456789123-0.123456789" <<endl;
            return;
        case '<':
            cout <<(A<B ? "TRUE" : "FALSE") <<endl;
            return;
        case '=':
            cout <<(A==B ? "TRUE" : "FALSE") <<endl;
            return;
        case '>':
            cout <<(A>B ? "TRUE" : "FALSE") <<endl;
            return;
        default:
            return;
    }
    cout <<A <<' ' <<opr <<' ' <<B <<" = ";
    cout <<C <<endl;
}


static const RationalNumber Rat_0 = RationalNumber(0);


int main()
{

    //test1();    // 加减
    //test2();    // 乘
    //test3();    // 除
    
    cout <<RationalNumber(0) <<endl;

    while (1) test4();    // 综合（binary operation）

    return 0;
}