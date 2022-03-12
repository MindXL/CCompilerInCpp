#include <iostream>

#include "Lexer.h"
#include "Parser.h"
#include "CodeGenerator.h"

using namespace CCC;

// TODO: check whether every shared_ptr.use_count is 1 after parsed.

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
