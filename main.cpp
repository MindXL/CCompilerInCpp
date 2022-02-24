#include <iostream>

#include"Lexer.h"

using namespace CCC;

const char *code = "   5+1-3*4/2";

int main() {
    Lexer lexer{code};

    while (!lexer.isEnd()) {
        lexer.getNextToken();
        std::cout << lexer.TEST_get_cur_content() << std::endl;
    }

    return 0;
}
