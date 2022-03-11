//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_PARSER_H
#define CCOMPILERINCPP_PARSER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "Lexer.h"
#include "AstNode.h"

#include "diagnose.h"

namespace CCC {
    class Parser {
    private:
        Lexer &lexer;
        std::list<std::shared_ptr<Identifier>> *p_locals{nullptr};
        std::unordered_map<std::string, std::shared_ptr<Identifier>> local_map;

    public:
        explicit Parser(Lexer &lexer) : lexer{lexer} {}

        // TODO: Maybe I can get rid of recursion.
        std::shared_ptr<ProgramNode> parse();

    private:
        std::shared_ptr<AstNode> parseFunctionDefinition();

        std::shared_ptr<AstNode> parseFunctionCall(std::shared_ptr<Token> &p_nameToken);

        std::shared_ptr<AstNode> parseStatement();

        std::shared_ptr<AstNode> parseExpr();

        std::shared_ptr<AstNode> parseAssignmentExpr();

        std::shared_ptr<AstNode> parseEqualityExpr();

        std::shared_ptr<AstNode> parseRelationalExpr();

        std::shared_ptr<AstNode> parseAddExpr();

        std::shared_ptr<AstNode> parseMulExpr();

        std::shared_ptr<AstNode> parsePrimaryExpr();

        std::shared_ptr<Identifier> findLocal(std::string &name);

        std::shared_ptr<Identifier> registerLocal(std::string &name);
    };
}


#endif //CCOMPILERINCPP_PARSER_H
