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

    REQUIRE(testLexer("  1+ 2-  3   * 4 /5") == "1+2-3*4/5");
    REQUIRE(testLexer("1 +(2 - 3 )*4/5") == "1+(2-3)*4/5");
    REQUIRE(testLexer("1 + 12") == "1+12");
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
        REQUIRE(testParser("1+2") == "21+");
        REQUIRE(testParser("1-2") == "21-");
        REQUIRE(testParser("1*2") == "21*");
        REQUIRE(testParser("1/2") == "21/");
        REQUIRE(testParser("1+2-3*4/5") == "543*/21+-");
        REQUIRE(testParser("1+23") == "231+");
        REQUIRE(testParser("12+3") == "312+");
    }

    SECTION("Parentheses") {
        REQUIRE(testParser("(1+2)") == "21+");
        REQUIRE(testParser("1+(2+3)") == "32+1+");
    }
}
