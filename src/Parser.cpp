//
// Created by mind on 2022/2/25.
//

#include "Parser.h"

using namespace CCC;

std::shared_ptr<ProgramNode> Parser::parse() {
    return std::make_shared<ProgramNode>(this->parseExpr());
}

std::shared_ptr<AstNode> Parser::parseExpr() {
    return this->parseAddExpr();
}

std::shared_ptr<AstNode> Parser::parseAddExpr() {
    std::shared_ptr<AstNode> left = this->parseMulExpr();

    while (this->lexer.p_token->type == TokenType::Add || this->lexer.p_token->type == TokenType::Sub) {
        BinaryOperator op = this->lexer.p_token->type == TokenType::Add ? BinaryOperator::Add : BinaryOperator::Sub;
        this->lexer.getNextToken();
        auto p_node = std::make_shared<BinaryNode>(op, std::move(left), this->parseMulExpr());

        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parseMulExpr() {
    std::shared_ptr<AstNode> left = this->parsePrimaryExpr();

    while (this->lexer.p_token->type == TokenType::Mul || this->lexer.p_token->type == TokenType::Div) {
        BinaryOperator op = this->lexer.p_token->type == TokenType::Mul ? BinaryOperator::Mul : BinaryOperator::Div;
        this->lexer.getNextToken();

        auto p_node = std::make_shared<BinaryNode>(op, std::move(left), this->parsePrimaryExpr());

        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parsePrimaryExpr() {
    if (this->lexer.p_token->type == TokenType::LParenthesis) {
        this->lexer.getNextToken();
        auto p_node = this->parseExpr();
        this->lexer.getNextToken();
        return p_node;
    } else {
        auto p_node = std::make_shared<ConstantNode>(this->lexer.p_token->value);

        this->lexer.getNextToken();
        return p_node;
    }
}
