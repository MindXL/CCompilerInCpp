#include "catch.hpp"

#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"

#include <string>

using namespace CCC;
using namespace std;

TEST_CASE("Lexer", "[Lexer]") {
    auto testLexer = [](const string &source) {
        Lexer lexer{source.c_str()};
        string result;

        while (!lexer.isEnd()) {
            lexer.getNextToken();
            result.append(lexer.p_token->content);
        }
        return result;
    };

    REQUIRE(testLexer("  1+ 2-  3   * 4 /5;") == "1+2-3*4/5;");    // regular use of binary operator

    SECTION("Parentheses") {
        REQUIRE(testLexer("1 +(2 - 3 )*4/5;") == "1+(2-3)*4/5;");
    }

    REQUIRE(testLexer("1 + 12;") == "1+12;");    // nums bigger than 9
    REQUIRE(testLexer("1+a/2;") == "1+a/2;");    // identifier(variable)
    REQUIRE(testLexer("ab;") == "ab;");    // complex identifier
    REQUIRE(testLexer("ab_;") == "ab_;");    // ...
    REQUIRE(testLexer("_ab012;") == "_ab012;");    // ...
    REQUIRE(testLexer("1+a/2;") == "1+a/2;");    // semicolon
    REQUIRE(testLexer("a = 1;") == "a=1;");    // assignment
    REQUIRE(testLexer("ab =1; ab+2+3; b=ab;c=ab*b;") == "ab=1;ab+2+3;b=ab;c=ab*b;");    // ...
    REQUIRE(testLexer("a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;") == "a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;");    // compare operator

    SECTION("Branch Statement") {
        REQUIRE(testLexer("a=3; if (a==1) a=1; else a=a*a;") == "a=3;if(a==1)a=1;elsea=a*a;");
        REQUIRE(testLexer("a=3; if (a==1) a=1; else if(a==2)a=2;else a=a*a;") ==
                "a=3;if(a==1)a=1;elseif(a==2)a=2;elsea=a*a;");
    }

    SECTION("While Statement") {
        REQUIRE(testLexer("a=0;while(a<10){a=a+1;a;}a;") == "a=0;while(a<10){a=a+1;a;}a;");
    }

    SECTION("Braces") {
        REQUIRE(testLexer("a=3; if (a==1) {a=1;} else if(a==2){a=2;}else {a=a*a;a=0;}") ==
                "a=3;if(a==1){a=1;}elseif(a==2){a=2;}else{a=a*a;a=0;}");
        REQUIRE(testLexer("a=1;if(a==1){}") == "a=1;if(a==1){}");
    }

    SECTION("Null Statement") {
        REQUIRE(testLexer("a=1;;a;") == "a=1;;a;");
    }
}

TEST_CASE("Parser", "[Parser]") {
    auto testParser = [](const string &source) {
        Lexer lexer{source.c_str()};
        lexer.getNextToken();

        Parser parser{lexer};
        auto root = parser.parse();

        PrintVisitor visitor;
        root->accept(&visitor);

        return visitor.content;
    };

    SECTION("Binary Operators") {
        REQUIRE(testParser("1+2;") == "1+2;");
        REQUIRE(testParser("1-2;") == "1-2;");
        REQUIRE(testParser("1*2;") == "1*2;");
        REQUIRE(testParser("1/2;") == "1/2;");
        REQUIRE(testParser("1+2-3*4/5;") == "1+2-3*4/5;");
        REQUIRE(testParser("1+23;") == "1+23;");
        REQUIRE(testParser("12+3;") == "12+3;");
        REQUIRE(testParser("a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;") == "a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;");
    }

//    SECTION("Parentheses") {
//        REQUIRE(testParser("(1+2);") == "1+2;");
//        REQUIRE(testParser("1+(2+3);") == "1+(2+3);");
//    }

    SECTION("Identifier") {
        REQUIRE(testParser("1+a/2;") == "1+a/2;");    // identifier(variable)
        REQUIRE(testParser("1+ab+2;") == "1+ab+2;");    // complex identifier
        REQUIRE(testParser("1-ab_/2;") == "1-ab_/2;");    // ...
        REQUIRE(testParser("100 * _ab012 / 20;") == "100*_ab012/20;");    // ...
    }

    SECTION("Semicolon") {
        REQUIRE(testParser("1+a/2;") == "1+a/2;");
        REQUIRE(testParser("1+a+b;2+c;") == "1+a+b;2+c;");
    }

    SECTION("Assignment") {
        REQUIRE(testParser("a = 1;") == "a=1;");
        REQUIRE(testParser("a=1;b=2;ab=1*2;") == "a=1;b=2;ab=1*2;");
        REQUIRE(testParser("ab =1; ab+2+3 ;a; b=ab;c=ab*b;") == "ab=1;ab+2+3;a;b=ab;c=ab*b;");
    }

    SECTION("Branch Statement") {
        REQUIRE(testParser("a=3; if (a==1) a=1; else a=a*a;") == "a=3;if(a==1)a=1;else a=a*a;");
        REQUIRE(testParser("a=3; if (a==1) a=1; else if(a==2)a=2;else a=a*a;") ==
                "a=3;if(a==1)a=1;else if(a==2)a=2;else a=a*a;");
    }

    SECTION("While Statement") {
        REQUIRE(testParser("a=0;while(a<10){a=a+1;a;}a;") == "a=0;while(a<10){a=a+1;a;}a;");
    }

    SECTION("Braces") {
        REQUIRE(testParser("a=3; if (a==1) {a=1;} else if(a==2){a=2;}else {a=a*a;a=0;}a=100;") ==
                "a=3;if(a==1){a=1;}else if(a==2){a=2;}else {a=a*a;a=0;}a=100;");
        REQUIRE(testParser("a=3; if (a==1) {a=1;} else {if(a==2){a=2;}else {a=a*a;a=0;}a=100;}") ==
                "a=3;if(a==1){a=1;}else {if(a==2){a=2;}else {a=a*a;a=0;}a=100;}");
        REQUIRE(testParser("a=1;if(a==1)a=2;else {a=3;}") == "a=1;if(a==1)a=2;else {a=3;}");
        REQUIRE(testParser("a=1;if(a==1){}") == "a=1;if(a==1){}");
    }

    SECTION("Null Statement") {
        REQUIRE(testParser("a=1;;a;") == "a=1;;a;");
    }
}
