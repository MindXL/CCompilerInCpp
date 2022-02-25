#include <iostream>

#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"
#include "CodeGenerator.h"

using namespace CCC;

const char *code = "   5+1-3*4/2";

void testLexer();

void testParser();

void testCodeGenerator();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "please input: ./ccc code" << std::endl;
        return 0;
    }

    const char *source = argv[1];
    Lexer lexer{source};
    lexer.getNextToken();

    Parser parser{lexer};
    CodeGenerator gen;

    auto root = parser.parse();
    root->accept(&gen);

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

void testCodeGenerator() {
    Lexer lexer{code};
    lexer.getNextToken();

    Parser parser{lexer};
    CodeGenerator gen;

    auto root = parser.parse();
    root->accept(&gen);
}