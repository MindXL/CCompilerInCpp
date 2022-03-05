//
// Created by mind on 2022/2/25.
//

#include "Parser.h"

#include <iostream>
#include <cassert>

using namespace CCC;

std::shared_ptr<ProgramNode> Parser::parse() {
    auto p_node = std::make_shared<ProgramNode>();
    p_locals = &p_node->locals;    // 绑定变量列表
    while (lexer.p_token->type != TokenType::Eof) {
        p_node->statements.emplace_back(parseStatementExpr());
    }
    return p_node;
}

std::shared_ptr<AstNode> Parser::parseStatementExpr() {
    auto &p_token = lexer.p_token;
    switch (p_token->type) {
        case TokenType::IF: {
            auto p_node = std::make_shared<IfStatementNode>();
            lexer.getNextToken();
            lexer.expectToken(TokenType::LParenthesis);
            lexer.getNextToken();
            p_node->condition_expr = parseExpr();
            lexer.expectToken(TokenType::RParenthesis);
            lexer.getNextToken();
            p_node->then_stmt = parseStatementExpr();
            if (p_token->type == TokenType::ELSE) {
                lexer.getNextToken();
                p_node->else_stmt = parseStatementExpr();
            }
            return p_node;
        }
        case TokenType::WHILE: {
            auto p_node = std::make_shared<WhileStatementNode>();
            lexer.getNextToken();
            lexer.expectToken(TokenType::LParenthesis);
            lexer.getNextToken();
            p_node->condition_expr = parseExpr();
            lexer.expectToken(TokenType::RParenthesis);
            lexer.getNextToken();
            p_node->then_stmt = parseStatementExpr();
            return p_node;
        }
        case TokenType::LBrace: {
            auto p_node = std::make_shared<BlockStatementNode>();
            lexer.getNextToken();
            while (p_token->type != TokenType::RBrace) {
                p_node->statements.emplace_back(parseStatementExpr());
            }
            lexer.expectToken(TokenType::RBrace);
            lexer.getNextToken();
            return p_node;
        }
        default: {
            auto p_node = std::make_shared<StatementNode>(
                    p_token->type != TokenType::Semicolon ? parseExpr() : nullptr
            );
            lexer.expectToken(TokenType::Semicolon);
            lexer.getNextToken();
            return p_node;
        }
    }
}

std::shared_ptr<AstNode> Parser::parseExpr() {
    return parseAssignmentExpr();
}

std::shared_ptr<AstNode> Parser::parseAssignmentExpr() {
    auto left = parseEqualityExpr();
    if (lexer.p_token->type == TokenType::Assignment) {
        lexer.getNextToken();
        return std::make_shared<AssignmentNode>(
                std::dynamic_pointer_cast<IdentifierNode>(left),
                parseAssignmentExpr()
        );
    } else {
        return left;
    }
}

std::shared_ptr<AstNode> Parser::parseEqualityExpr() {
    auto left = parseRelationalExpr();
    auto &type = lexer.p_token->type;
    while (type == TokenType::EQ || type == TokenType::NE) {
        BinaryOperator op = type == TokenType::EQ ? BinaryOperator::EQ : BinaryOperator::NE;
        lexer.getNextToken();
        auto p_node = std::make_shared<BinaryNode>(op, std::move(left), parseEqualityExpr());
        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parseRelationalExpr() {
    auto left = parseAddExpr();
    auto &type = lexer.p_token->type;
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
        lexer.getNextToken();
        auto p_node = std::make_shared<BinaryNode>(op, std::move(left), parseRelationalExpr());
        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parseAddExpr() {
    auto left = parseMulExpr();

    auto &type = lexer.p_token->type;
    while (type == TokenType::Add || type == TokenType::Sub) {
        BinaryOperator op = type == TokenType::Add ? BinaryOperator::Add : BinaryOperator::Sub;
        lexer.getNextToken();
        auto p_node = std::make_shared<BinaryNode>(
                op,
                std::move(left),
                parseAddExpr()
        );

        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parseMulExpr() {
    auto left = parsePrimaryExpr();

    auto &type = lexer.p_token->type;
    while (type == TokenType::Mul || type == TokenType::Div) {
        BinaryOperator op = type == TokenType::Mul ? BinaryOperator::Mul : BinaryOperator::Div;
        lexer.getNextToken();

        auto p_node = std::make_shared<BinaryNode>(op, std::move(left), parseMulExpr());

        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parsePrimaryExpr() {
    std::shared_ptr<AstNode> p_node{nullptr};
    switch (lexer.p_token->type) {
        case TokenType::LParenthesis: {
            lexer.getNextToken();
            p_node = parseExpr();
            lexer.expectToken(TokenType::RParenthesis);
            break;
        }
        case TokenType::Num: {
            p_node = std::make_shared<ConstantNode>(lexer.p_token->value);
            break;
        }
        case TokenType::Identifier: {
            auto name = lexer.p_token->content;
            auto identifier = findLocal(name);
            p_node = std::make_shared<IdentifierNode>(identifier ? identifier : registerLocal(name));
            break;
        }
        case TokenType::Eof: {
            auto &p_token = lexer.p_token;
            diagnose(lexer.source, lexer.line_head, p_token->location.line_num, p_token->location.col_num,
                     p_token->content.size(),
                     "unexpected end of file.");
        }
        default: {
            auto &p_token = lexer.p_token;
            diagnose(lexer.source, lexer.line_head, p_token->location.line_num, p_token->location.col_num,
                     p_token->content.size(),
                     "grammar around this token '", p_token->content, "' is not supported.");
        }
    }
    lexer.getNextToken();
    return p_node;
}

std::shared_ptr<Identifier> Parser::findLocal(std::string_view &name) {
    auto cit = local_map.find(name);
    return cit == local_map.cend() ? nullptr : cit->second;
}

std::shared_ptr<Identifier> Parser::registerLocal(std::string_view &name) {
    auto p_local = std::make_shared<Identifier>(name);
    p_locals->emplace_back(p_local);
    local_map.emplace(name, p_local);
    return p_local;
}
