#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>


class A {
public:
    virtual ~A() = default;
    virtual std::string to_string() const = 0;
};

class A_int : public A {
public:
    int val;
    A_int(int v) : val(v) {}
};

class A_double : public A {
public:
    double val;
    A_double(double v) : val(v) {}
};


class A_my;
namespace std {
    string to_string (const A_my &X);
}

class A_my : public A {
public:
    double v1, v2;
    A_my(double v1, double v2) : v1(v1), v2(v2) {}
    friend std::string std::to_string(const A_my &X);
};


std::string std::to_string(const A_my &X)
{
    return std::to_string(X.v1)+'/'+std::to_string(X.v2);
}



void fun(std::string &str, const A *X)
{
    str += std::to_string(*X).append(' ');
}


void test()
{
    A_int aint(15);
    A_double adouble(2.3);
    A_my amy(6, 7);
    std::string str;

    fun(str, &aint);
    fun(str, &adouble);
    fun(str, &amy);

    std::cout <<str <<'\n';
}




int main()
{
    test();
    return 0;
}