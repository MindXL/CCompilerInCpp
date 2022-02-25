//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_PARSER_H
#define CCOMPILERINCPP_PARSER_H

#include <memory>

#include "Lexer.h"
#include "AstNode.h"

namespace CCC {
    class Parser {
    private:
        Lexer &lexer;

    public:
        explicit Parser(Lexer &lexer) : lexer{lexer} {}

        std::shared_ptr<ProgramNode> parse();

    private:
        std::shared_ptr<AstNode> parseExpr();

        std::shared_ptr<AstNode> parseAddExpr();

        std::shared_ptr<AstNode> parseMulExpr();

        std::shared_ptr<AstNode> parsePrimaryExpr();
    };
}


#endif //CCOMPILERINCPP_PARSER_H
