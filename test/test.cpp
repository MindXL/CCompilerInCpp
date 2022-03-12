#include "catch.hpp"

#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"

#include <string>
#include <fstream>
#include <filesystem>

using namespace CCC;
using namespace std;

const char *testfile = "test.c";

// TODO: modify test framework
TEST_CASE("Lexer", "[Lexer]") {
    auto T = [](const char *source) {
        std::fstream fs(testfile, std::ios::out | std::ios::trunc);
        fs << source;
        fs.close();

        Lexer lexer{testfile};
        string result;

        while (!lexer.isEnd()) {
            lexer.getNextToken();
            result.append(lexer.p_token->content);
        }
        return result;
    };

    REQUIRE(T("  1+ 2-  3   * 4 /5;") == "1+2-3*4/5;");    // regular use of binary operator

    SECTION("Parentheses") {
        REQUIRE(T("1 +(2 - 3 )*4/5;") == "1+(2-3)*4/5;");
    }

    REQUIRE(T("1 + 12;") == "1+12;");    // nums bigger than 9
    REQUIRE(T("1+a/2;") == "1+a/2;");    // identifier(variable)
    REQUIRE(T("ab;") == "ab;");    // complex identifier
    REQUIRE(T("ab_;") == "ab_;");    // ...
    REQUIRE(T("_ab012;") == "_ab012;");    // ...
    REQUIRE(T("1+a/2;") == "1+a/2;");    // semicolon
    REQUIRE(T("a = 1;") == "a=1;");    // assignment
    REQUIRE(T("ab =1; ab+2+3; b=ab;c=ab*b;") == "ab=1;ab+2+3;b=ab;c=ab*b;");    // ...
    REQUIRE(T("a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;") == "a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;");    // compare operator

    SECTION("Branch Statement") {
        REQUIRE(T("a=3; if (a==1) a=1; else a=a*a;") == "a=3;if(a==1)a=1;elsea=a*a;");
        REQUIRE(T("a=3; if (a==1) a=1; else if(a==2)a=2;else a=a*a;") == "a=3;if(a==1)a=1;elseif(a==2)a=2;elsea=a*a;");
    }

    SECTION("While Statement") {
        REQUIRE(T("a=0;while(a<10){a=a+1;a;}a;") == "a=0;while(a<10){a=a+1;a;}a;");
    }

    SECTION("For Statement") {
        REQUIRE(T("a=1;for(i=1;i<=10;i=i+1) a=a+1;") == "a=1;for(i=1;i<=10;i=i+1)a=a+1;");
        REQUIRE(T("a=1;i=1;for(;i<=10;i=i+1) a=a+1;") == "a=1;i=1;for(;i<=10;i=i+1)a=a+1;");
//        REQUIRE(T()==);    // expr2 not testable for now
        REQUIRE(T("a=1;i=1;for(;i<=10;) {a=a+1;i=i+1;}") == "a=1;i=1;for(;i<=10;){a=a+1;i=i+1;}");
    }

    SECTION("Do While Statament") {
        REQUIRE(T("a=0;do a=a+1;while(a<10);") == "a=0;doa=a+1;while(a<10);");
        REQUIRE(T("a=0;do {a=a+1; a=a+2;}while(a<10);") == "a=0;do{a=a+1;a=a+2;}while(a<10);");
    }

    SECTION("Braces") {
        REQUIRE(T("a=3; if (a==1) {a=1;} else if(a==2){a=2;}else {a=a*a;a=0;}") ==
                "a=3;if(a==1){a=1;}elseif(a==2){a=2;}else{a=a*a;a=0;}");
        REQUIRE(T("a=1;if(a==1){}") == "a=1;if(a==1){}");
    }

    SECTION("Null Statement") {
        REQUIRE(T("a=1;;a;") == "a=1;;a;");
    }

    SECTION("Function Definition") {
        REQUIRE(T("func(){}") == "func(){}");
        REQUIRE(T("func(a){}") == "func(a){}");
        REQUIRE(T("func(a, b){}") == "func(a,b){}");
        REQUIRE(T("func(a, b){a=1;b==2;}") == "func(a,b){a=1;b==2;}");
        REQUIRE(T("func(){{}{{}}}") == "func(){{}{{}}}");
    }

    SECTION("Function Call") {
        REQUIRE(T("f(n) {n=n+1;} g(n) {n=n+2;} t(){f(1);}") == "f(n){n=n+1;}g(n){n=n+2;}t(){f(1);}");
        REQUIRE(T("sum(n){sum=0;for(i=0;i<=n;i=i+1){sum=sum+i;}sum;}prog(){sum(100);}") ==
                "sum(n){sum=0;for(i=0;i<=n;i=i+1){sum=sum+i;}sum;}prog(){sum(100);}");
        REQUIRE(T("add(a,b,c,d,e,f){a+b+c+d+e+f;}prog(){add(1,2,3,4,5,6);}") ==
                "add(a,b,c,d,e,f){a+b+c+d+e+f;}prog(){add(1,2,3,4,5,6);}");
        //        REQUIRE(T("f(n) {n=n+1;} g(n) {n=n+2;} g(f(1));") == "f(n){n=n+1;}g(n){n=n+2;}g(f(1));");
    }

    SECTION("Return Statement") {
        REQUIRE(T("f(){return 1;}") == "f(){return1;}");
        REQUIRE(T("f(n){return n;}") == "f(n){returnn;}");
        REQUIRE(T("add(a,b,c,d,e,f){return a+b+c+d+e+f;}prog(){return add(1,2,3,4,5,6);}") ==
                "add(a,b,c,d,e,f){returna+b+c+d+e+f;}prog(){returnadd(1,2,3,4,5,6);}");
        REQUIRE(T("fib(n){if(n<=1){return 1;}return fib(n-1)+fib(n-2);}prog(){return fib(4);}") ==
                "fib(n){if(n<=1){return1;}returnfib(n-1)+fib(n-2);}prog(){returnfib(4);}");
    }

    filesystem::remove(testfile);
}

TEST_CASE("Parser", "[Parser]") {
    auto T = [](const char *source) {
        std::fstream fs(testfile, std::ios::out | std::ios::trunc);
        fs << source;
        fs.close();

        Lexer lexer{testfile};
        lexer.getNextToken();

        Parser parser{lexer};
        auto root = parser.parse();

        PrintVisitor visitor;
        root->accept(&visitor);

        return visitor.content;
    };

    /*
    SECTION("Binary Operators") {
        REQUIRE(T("1+2;") == "1+2;");
        REQUIRE(T("1-2;") == "1-2;");
        REQUIRE(T("1*2;") == "1*2;");
        REQUIRE(T("1/2;") == "1/2;");
        REQUIRE(T("1+2-3*4/5;") == "1+2-3*4/5;");
        REQUIRE(T("1+23;") == "1+23;");
        REQUIRE(T("12+3;") == "12+3;");
        REQUIRE(T("a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;") == "a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;");
    }

    SECTION("Parentheses") {
        REQUIRE(testParser("(1+2);") == "(1+2);");
        REQUIRE(testParser("1+(2+3);") == "1+(2+3);");
    }

    SECTION("Identifier") {
        REQUIRE(T("1+a/2;") == "1+a/2;");    // identifier(variable)
        REQUIRE(T("1+ab+2;") == "1+ab+2;");    // complex identifier
        REQUIRE(T("1-ab_/2;") == "1-ab_/2;");    // ...
        REQUIRE(T("100 * _ab012 / 20;") == "100*_ab012/20;");    // ...
    }

    SECTION("Semicolon") {
        REQUIRE(T("1+a/2;") == "1+a/2;");
        REQUIRE(T("1+a+b;2+c;") == "1+a+b;2+c;");
    }

    SECTION("Assignment") {
        REQUIRE(T("a = 1;") == "a=1;");
        REQUIRE(T("a=1;b=2;ab=1*2;") == "a=1;b=2;ab=1*2;");
        REQUIRE(T("ab =1; ab+2+3 ;a; b=ab;c=ab*b;") == "ab=1;ab+2+3;a;b=ab;c=ab*b;");
    }

    SECTION("Branch Statement") {
        REQUIRE(T("a=3; if (a==1) a=1; else a=a*a;") == "a=3;if(a==1)a=1;else a=a*a;");
        REQUIRE(T("a=3; if (a==1) a=1; else if(a==2)a=2;else a=a*a;") ==
                "a=3;if(a==1)a=1;else if(a==2)a=2;else a=a*a;");
    }

    SECTION("While Statement") {
        REQUIRE(T("a=0;while(a<10){a=a+1;a;}a;") == "a=0;while(a<10){a=a+1;a;}a;");
    }

    SECTION("Do While Statament") {
        REQUIRE(T("a=0;do a=a+1;while(a<10);") == "a=0;do a=a+1;while(a<10);");
        REQUIRE(T("a=0;do {a=a+1; a=a+2;}while(a<10);") == "a=0;do {a=a+1;a=a+2;}while(a<10);");
    }

    SECTION("For Statement") {
        REQUIRE(T("a=1;for(i=1;i<=10;i=i+1) a=a+1;") == "a=1;for(i=1;i<=10;i=i+1)a=a+1;");
        REQUIRE(T("a=1;i=1;for(;i<=10;i=i+1) a=a+1;") == "a=1;i=1;for(;i<=10;i=i+1)a=a+1;");
//        REQUIRE(T()==);    // expr2 not testable for now
        REQUIRE(T("a=1;i=1;for(;i<=10;) {a=a+1;i=i+1;}") == "a=1;i=1;for(;i<=10;){a=a+1;i=i+1;}");
    }

    SECTION("Braces") {
        REQUIRE(T("a=3; if (a==1) {a=1;} else if(a==2){a=2;}else {a=a*a;a=0;}a=100;") ==
                "a=3;if(a==1){a=1;}else if(a==2){a=2;}else {a=a*a;a=0;}a=100;");
        REQUIRE(T("a=3; if (a==1) {a=1;} else {if(a==2){a=2;}else {a=a*a;a=0;}a=100;}") ==
                "a=3;if(a==1){a=1;}else {if(a==2){a=2;}else {a=a*a;a=0;}a=100;}");
        REQUIRE(T("a=1;if(a==1)a=2;else {a=3;}") == "a=1;if(a==1)a=2;else {a=3;}");
        REQUIRE(T("a=1;if(a==1){}") == "a=1;if(a==1){}");
    }

    SECTION("Null Statement") {
        REQUIRE(T("a=1;;a;") == "a=1;;a;");
    }
    */

    SECTION("Function Definition") {
        REQUIRE(T("func(){}") == "func(){}");
        REQUIRE(T("func(a){}") == "func(a){}");
        REQUIRE(T("func(a, b){}") == "func(a,b){}");
        REQUIRE(T("func(a, b){a=1;b==2;}") == "func(a,b){a=1;b==2;}");
        REQUIRE(T("func(){{}{{}}}") == "func(){{}{{}}}");
    }

    SECTION("Function Call") {
        REQUIRE(T("f(n) {n=n+1;} g(n) {n=n+2;} t(){f(1);}") == "f(n){n=n+1;}g(n){n=n+2;}t(){f(1);}");
        REQUIRE(T("sum(n){sum=0;for(i=0;i<=n;i=i+1){sum=sum+i;}sum;}prog(){sum(100);}") ==
                "sum(n){sum=0;for(i=0;i<=n;i=i+1){sum=sum+i;}sum;}prog(){sum(100);}");
        REQUIRE(T("add(a,b,c,d,e,f){a+b+c+d+e+f;}prog(){add(1,2,3,4,5,6);}") ==
                "add(a,b,c,d,e,f){a+b+c+d+e+f;}prog(){add(1,2,3,4,5,6);}");
//        REQUIRE(T("f(n) {n=n+1;} g(n) {n=n+2;} g(f(1));") == "f(n){n=n+1;}g(n){n=n+2;}g(f(1));");
    }

    SECTION("Return Statement") {
        REQUIRE(T("f(){return 1;}") == "f(){return 1;}");
        REQUIRE(T("f(n){return n;}") == "f(n){return n;}");
        REQUIRE(T("add(a,b,c,d,e,f){return a+b+c+d+e+f;}prog(){return add(1,2,3,4,5,6);}") ==
                "add(a,b,c,d,e,f){return a+b+c+d+e+f;}prog(){return add(1,2,3,4,5,6);}");
        REQUIRE(T("fib(n){if(n<=1){return 1;}return fib(n-1)+fib(n-2);}prog(){return fib(4);}") ==
                "fib(n){if(n<=1){return 1;}return fib(n-1)+fib(n-2);}prog(){return fib(4);}");
    }

    filesystem::remove(testfile);
}
