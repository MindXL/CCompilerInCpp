//
// Created by mind on 2022/2/25.
//

#include "Parser.h"

#include <iostream>
#include <cassert>

using namespace CCC;

std::shared_ptr<ProgramNode> Parser::parse() {
    auto p_node = std::make_shared<ProgramNode>();
    while (lexer.p_token->type != TokenType::Eof) {
        p_node->functions.emplace_back(parseFunction());
    }
    return p_node;
}

std::shared_ptr<AstNode> Parser::parseFunction() {
    auto p_node = std::make_shared<FunctionNode>();
    p_locals = &p_node->locals;    // 绑定变量列表
    local_map.clear();

    p_node->name = lexer.p_token->content;
    lexer.expectToken(TokenType::Identifier);
    lexer.expectToken(TokenType::LParenthesis);
    if (lexer.p_token->type != TokenType::RParenthesis) {
        // have parameters
        do {
            auto p_token = lexer.p_token;
            auto line = lexer.line;
            parsePrimaryExpr();
            auto p_identifier = findLocal(p_token->content);
            if (!p_identifier) {
                diagnose(line, p_token->location.n_line, p_token->location.start_pos, p_token->content.length(),
                         p_token->content, " is not a valid declaration of a parameter.");
            }
            p_node->parameters.emplace_back(p_identifier);
        } while (lexer.p_token->type == TokenType::Comma && lexer.expectToken(TokenType::Comma));
    }
    lexer.expectToken(TokenType::RParenthesis);

    lexer.expectToken(TokenType::LBrace);
    while (lexer.p_token->type != TokenType::RBrace) {
        p_node->statements.emplace_back(parseStatement());
    }
    lexer.expectToken(TokenType::RBrace);

    return p_node;
}

std::shared_ptr<AstNode> Parser::parseFunctionCall(std::shared_ptr<Token> &p_nameToken) {
    auto p_node = std::make_shared<FunctionCallNode>(p_nameToken->content);
    lexer.expectToken(TokenType::LParenthesis);
    if (lexer.p_token->type != TokenType::RParenthesis) {
        // have arguments
        do {
            p_node->arguments.emplace_back(parseAssignmentExpr());
        } while (lexer.p_token->type == TokenType::Comma && lexer.expectToken(TokenType::Comma));
    }
    lexer.expectToken(TokenType::RParenthesis);
    return p_node;
}

std::shared_ptr<AstNode> Parser::parseStatement() {
    auto &p_token = lexer.p_token;
    switch (p_token->type) {
        case TokenType::IF: {
            auto p_node = std::make_shared<IfStatementNode>();
            lexer.getNextToken();
            lexer.expectToken(TokenType::LParenthesis);
            p_node->condition_expr = parseExpr();
            lexer.expectToken(TokenType::RParenthesis);
            p_node->then_stmt = parseStatement();
            if (p_token->type == TokenType::ELSE) {
                lexer.getNextToken();
                p_node->else_stmt = parseStatement();
            }
            return p_node;
        }
        case TokenType::WHILE: {
            auto p_node = std::make_shared<WhileStatementNode>();
            lexer.getNextToken();
            lexer.expectToken(TokenType::LParenthesis);
            p_node->condition_expr = parseExpr();
            lexer.expectToken(TokenType::RParenthesis);
            p_node->then_stmt = parseStatement();
            return p_node;
        }
        case TokenType::DO: {
            auto p_node = std::make_shared<DoWhileStatementNode>();
            lexer.getNextToken();
            p_node->then_stmt = parseStatement();
            lexer.expectToken(TokenType::WHILE);
            lexer.expectToken(TokenType::LParenthesis);
            p_node->condition_expr = parseExpr();
            lexer.expectToken(TokenType::RParenthesis);
            lexer.expectToken(TokenType::Semicolon);
            return p_node;
        }
        case TokenType::FOR: {
            auto p_node = std::make_shared<ForStatementNode>();
            lexer.getNextToken();
            lexer.expectToken(TokenType::LParenthesis);
            if (p_token->type != TokenType::Semicolon)
                p_node->expr1 = parseExpr();
            lexer.expectToken(TokenType::Semicolon);
            if (p_token->type != TokenType::Semicolon)
                p_node->expr2 = parseExpr();
            lexer.expectToken(TokenType::Semicolon);
            if (p_token->type != TokenType::RParenthesis)
                p_node->expr3 = parseExpr();
            lexer.expectToken(TokenType::RParenthesis);
            p_node->then_stmt = parseStatement();
            return p_node;
        }
        case TokenType::LBrace: {
            auto p_node = std::make_shared<BlockStatementNode>();
            lexer.getNextToken();
            while (p_token->type != TokenType::RBrace) {
                p_node->statements.emplace_back(parseStatement());
            }
            lexer.expectToken(TokenType::RBrace);
            return p_node;
        }
        default: {
            auto p_node = std::make_shared<StatementNode>(
                    p_token->type != TokenType::Semicolon ? parseExpr() : nullptr
            );
            lexer.expectToken(TokenType::Semicolon);
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
    auto &p_token = lexer.p_token;
    while (p_token->type == TokenType::EQ || p_token->type == TokenType::NE) {
        BinaryOperator op = p_token->type == TokenType::EQ ? BinaryOperator::EQ : BinaryOperator::NE;
        lexer.getNextToken();
        auto p_node = std::make_shared<BinaryNode>(op, std::move(left), parseEqualityExpr());
        left = p_node;
    }
    return left;
}

std::shared_ptr<AstNode> Parser::parseRelationalExpr() {
    auto left = parseAddExpr();
    auto &p_token = lexer.p_token;
    while (p_token->type == TokenType::GT || p_token->type == TokenType::GE || p_token->type == TokenType::LT ||
           p_token->type == TokenType::LE) {
        BinaryOperator op;
        switch (p_token->type) {
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

    auto &p_token = lexer.p_token;
    while (p_token->type == TokenType::Add || p_token->type == TokenType::Sub) {
        BinaryOperator op = p_token->type == TokenType::Add ? BinaryOperator::Add : BinaryOperator::Sub;
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

    auto &p_token = lexer.p_token;
    while (p_token->type == TokenType::Mul || p_token->type == TokenType::Div) {
        BinaryOperator op = p_token->type == TokenType::Mul ? BinaryOperator::Mul : BinaryOperator::Div;
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
            lexer.getNextToken();
            break;
        }
        case TokenType::Identifier: {
            auto p_lastToken = lexer.p_token;
            lexer.getNextToken();
            if (lexer.p_token->type == TokenType::LParenthesis) {
                // func call
                return parseFunctionCall(p_lastToken);
            }

            auto name = p_lastToken->content;
            auto identifier = findLocal(name);
            p_node = std::make_shared<IdentifierNode>(identifier ? identifier : registerLocal(name));
            break;
        }
        case TokenType::Eof: {
            auto &p_token = lexer.p_token;
            diagnose(lexer.line, p_token->location.n_line, lexer.line.length(), 0,
                     "unexpected end of file.");
        }
        default: {
            auto &p_token = lexer.p_token;
            diagnose(lexer.line, p_token->location.n_line, p_token->location.start_pos, p_token->content.length(),
                     "grammar around this token '", p_token->content, "' is not supported.");
        }
    }
    return p_node;
}

std::shared_ptr<Identifier> Parser::findLocal(std::string &name) {
    auto cit = local_map.find(name);
    return cit == local_map.cend() ? nullptr : cit->second;
}

std::shared_ptr<Identifier> Parser::registerLocal(std::string &name) {
    auto p_local = std::make_shared<Identifier>(name);
    p_locals->emplace_back(p_local);
    local_map.emplace(name, p_local);
    return p_local;
}
