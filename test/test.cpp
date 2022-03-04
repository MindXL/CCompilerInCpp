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
    REQUIRE(testLexer("1 +(2 - 3 )*4/5;") == "1+(2-3)*4/5;");    // parentheses
    REQUIRE(testLexer("1 + 12;") == "1+12;");    // nums bigger than 9
    REQUIRE(testLexer("1+a/2;") == "1+a/2;");    // identifier(variable)
    REQUIRE(testLexer("ab;") == "ab;");    // complex identifier
    REQUIRE(testLexer("ab_;") == "ab_;");    // ...
    REQUIRE(testLexer("_ab012;") == "_ab012;");    // ...
    REQUIRE(testLexer("1+a/2;") == "1+a/2;");    // semicolon
    REQUIRE(testLexer("a = 1;") == "a=1;");    // assignment
    REQUIRE(testLexer("ab =1; ab+2+3; b=ab;c=ab*b;") == "ab=1;ab+2+3;b=ab;c=ab*b;");    // ...
    REQUIRE(testLexer("a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;") == "a=0;a==0;a!=0;a>0;a>=0;a<0;a<=0;");    // compare operator
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
}
