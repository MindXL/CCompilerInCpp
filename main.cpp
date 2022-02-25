#include <iostream>

#include"Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"

using namespace CCC;

const char *code = "   5+1-3*4/2";

void testLexer();

void testParser();

int main() {
    testParser();

    return 0;
}

void testLexer() {
    Lexer lexer{code};

    while (!lexer.isEnd()) {
        lexer.getNextToken();
        std::cout << lexer.TEST_get_cur_content() << std::endl;
    }
}

void testParser() {
    Lexer lexer{code};
    lexer.getNextToken();

    Parser parser{lexer};
    PrintVisitor visitor;

    auto root = parser.parse();
    root->accept(&visitor);
}