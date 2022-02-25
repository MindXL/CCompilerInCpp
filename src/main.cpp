#include <iostream>

#include "Lexer.h"
#include "Parser.h"
#include "CodeGenerator.h"

using namespace CCC;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Please input: ./ccc code" << std::endl;
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
