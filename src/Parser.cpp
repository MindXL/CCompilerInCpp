//
// Created by mind on 2022/2/25.
//

#include "Parser.h"

#include <iostream>
#include <cassert>

using namespace CCC;

std::shared_ptr<ProgramNode> Parser::parse() {
    auto p_node = std::make_shared<ProgramNode>();
    this->p_locals = &p_node->locals;    // 绑定变量列表
    while (this->lexer.p_token->type != TokenType::Eof) {
        p_node->statements.emplace_back(this->parseStatementExpr());
    }
    return p_node;
}

std::shared_ptr<AstNode> Parser::parseStatementExpr() {
    auto p_node = std::make_shared<StatementNode>(this->parseExpr());
    this->lexer.expectToken(TokenType::Semicolon);
    this->lexer.getNextToken();
    return p_node;
}

std::shared_ptr<AstNode> Parser::parseExpr() {
    return this->parseAssignmentExpr();
}

std::shared_ptr<AstNode> Parser::parseAssignmentExpr() {
    auto left = this->parseEqualityExpr();
    if (this->lexer.p_token->type == TokenType::Assignment) {
        this->lexer.getNextToken();
        return std::make_shared<AssignmentNode>(
                std::dynamic_pointer_cast<IdentifierNode>(left),
                this->parseAssignmentExpr()
        );
    } else {
        return left;
    }
}

std::shared_ptr<AstNode> Parser::parseEqualityExpr() {
    auto left = this->parseRelationalExpr();
    auto &type = this->lexer.p_token->type;
    while (type == TokenType::EQ || type == TokenType::NE) {
        BinaryOperator op = type == TokenType::EQ ? BinaryOperator::EQ : BinaryOperator::NE;
        this->lexer.getNextToken();
        auto p_node = std::make_shared<BinaryNode>(op, std::move(left), this->parseEqualityExpr());
        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parseRelationalExpr() {
    auto left = this->parseAddExpr();
    auto &type = this->lexer.p_token->type;
    while (type == TokenType::GT || type == TokenType::GE || type == TokenType::LT || type == TokenType::LE) {
        BinaryOperator op;
        switch (type) {
            case TokenType::GT:
                op = BinaryOperator::GT;
                break;
            case TokenType::GE:
                op = BinaryOperator::GE;
                break;
            case TokenType::LT:
                op = BinaryOperator::LT;
                break;
            case TokenType::LE:
                op = BinaryOperator::LE;
                break;
            default:
                assert(0);
        }
        this->lexer.getNextToken();
        auto p_node = std::make_shared<BinaryNode>(op, std::move(left), this->parseRelationalExpr());
        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parseAddExpr() {
    auto left = this->parseMulExpr();

    auto &type = this->lexer.p_token->type;
    while (type == TokenType::Add || type == TokenType::Sub) {
        BinaryOperator op = type == TokenType::Add ? BinaryOperator::Add : BinaryOperator::Sub;
        this->lexer.getNextToken();
        auto p_node = std::make_shared<BinaryNode>(
                op,
                std::move(left),
                this->parseAddExpr()
        );

        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parseMulExpr() {
    auto left = this->parsePrimaryExpr();

    auto &type = this->lexer.p_token->type;
    while (type == TokenType::Mul || type == TokenType::Div) {
        BinaryOperator op = type == TokenType::Mul ? BinaryOperator::Mul : BinaryOperator::Div;
        this->lexer.getNextToken();

        auto p_node = std::make_shared<BinaryNode>(op, std::move(left), this->parseMulExpr());

        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parsePrimaryExpr() {
    std::shared_ptr<AstNode> p_node{nullptr};
    switch (this->lexer.p_token->type) {
        case TokenType::LParenthesis: {
            this->lexer.getNextToken();
            p_node = this->parseExpr();
            this->lexer.expectToken(TokenType::RParenthesis);
            break;
        }
        case TokenType::Num: {
            p_node = std::make_shared<ConstantNode>(this->lexer.p_token->value);
            break;
        }
        case TokenType::Identifier: {
            auto name = this->lexer.p_token->content;
            auto identifier = this->findLocal(name);
            p_node = std::make_shared<IdentifierNode>(identifier ? identifier : this->registerLocal(name));
            break;
        }
        default: {
            auto &p_token = this->lexer.p_token;
            diagnose(this->lexer.source, this->lexer.line_head, p_token->location.line_num, p_token->location.col_num,
                     p_token->content.size(),
                     "grammar around this token '", p_token->content, "' is not supported.\n");
        }
    }
    this->lexer.getNextToken();
    return p_node;
}

std::shared_ptr<Identifier> Parser::findLocal(std::string_view &name) {
    auto cit = this->local_map.find(name);
    return cit == this->local_map.cend() ? nullptr : cit->second;
}

std::shared_ptr<Identifier> Parser::registerLocal(std::string_view &name) {
    auto p_local = std::make_shared<Identifier>(name);
    this->p_locals->emplace_back(p_local);
    this->local_map.emplace(name, p_local);
    return p_local;
}
