#include <iostream>
#include <fstream>

#include "Lexer.h"
#include "Parser.h"
#include "CodeGenerator.h"

using namespace CCC;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Please input: ./ccc filename" << std::endl;
        return 1;
    }

    const char *filename = argv[1];
    Lexer lexer{filename};
    lexer.getNextToken();

    Parser parser{lexer};
    CodeGenerator gen;

    auto root = parser.parse();
    root->accept(&gen);

    return 0;
}
