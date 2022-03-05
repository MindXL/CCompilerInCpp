//
// Created by mind on 2022/2/25.
//

#include "Lexer.h"

#include <sstream>
#include <cassert>

using namespace CCC;

bool Lexer::isEnd() {
    return cit == source.cend();
}

void Lexer::getNextToken() {
    /// skip the whitespace
    while (cit != source.cend() && isspace(*cit)) {
        if (*cit == '\n') {
            line_num++;
            line_head = cit;
            line_head++;
        }
        cit++;
    }

    TokenType type;
    TokenLocation location{line_num, std::distance(line_head, cit)};

    int value = 0;
    const auto start = cit;

    if (cit == source.cend()) { type = TokenType::Eof; }
    else if (*cit == '+') {
        cit++;
        type = TokenType::Add;
    } else if (*cit == '-') {
        cit++;
        type = TokenType::Sub;
    } else if (*cit == '*') {
        cit++;
        type = TokenType::Mul;
    } else if (*cit == '/') {
        cit++;
        type = TokenType::Div;
    } else if (*cit == '(') {
        cit++;
        type = TokenType::LParenthesis;
    } else if (*cit == ')') {
        cit++;
        type = TokenType::RParenthesis;
    } else if (*cit == '{') {
        cit++;
        type = TokenType::LBrace;
    } else if (*cit == '}') {
        cit++;
        type = TokenType::RBrace;
    } else if (*cit == ';') {
        cit++;
        type = TokenType::Semicolon;
    } else if (*cit == '=') {
        cit++;
        if (peekChar() == '=') {
            type = TokenType::EQ;
            cit++;
        } else {
            type = TokenType::Assignment;
        }
    } else if (*cit == '!') {
        cit++;
        if (peekChar() == '=') {
            type = TokenType::NE;
            cit++;
        } else {
            diagnose(source, line_head, location.line_num, location.col_num, p_token->content.size(),
                     "unexpected '", *cit, '\'');
        }
    } else if (*cit == '>') {
        cit++;
        if (peekChar() == '=') {
            type = TokenType::GE;
            cit++;
        } else {
            type = TokenType::GT;
        }
    } else if (*cit == '<') {
        cit++;
        if (peekChar() == '=') {
            type = TokenType::LE;
            cit++;
        } else {
            type = TokenType::LT;
        }
    } else if (isdigit(*cit)) {
        type = TokenType::Num;
        std::stringstream ss;
        do {
            ss << *cit;
            cit++;
        } while (!isEnd() && isdigit(*cit));
        ss >> value;
    } else if (isValidIdentifierLetter()) {
        do {
            cit++;
        } while (!isEnd() && (isValidIdentifierLetter() || isdigit(*cit)));
        auto content = source.substr(
                std::distance(source.cbegin(), start),
                std::distance(start, cit)
        );
        if (content == "if") {
            type = TokenType::IF;
        } else if (content == "else") {
            type = TokenType::ELSE;
        } else if (content == "while") {
            type = TokenType::WHILE;
        } else {
            type = TokenType::Identifier;
        }
    } else {
        diagnose(source, line_head, location.line_num, location.col_num, p_token->content.size(),
                 "unexpected '", *cit, '\'');
    }

    p_token = std::make_shared<Token>(
            type,
            value,
            source.substr(
                    std::distance(source.cbegin(), start),
                    std::distance(start, cit)
            ),
            location
    );
}

void Lexer::expectToken(TokenType type) {
    // This function won't pass the current token.
    // After 'expectToken', 'getNextToken' is needed provided to pass the current token.
    if (p_token->type != type) {
        char expected{'\0'};
        switch (type) {
            case TokenType::Semicolon:
                expected = ';';
                break;
            case TokenType::RParenthesis:
                expected = ')';
                break;
            default:
                assert(0);
        }
        const auto&[line_num, col_num]=p_token->location;
        diagnose(source, line_head, line_num, col_num, 0,
                 "expected '", expected, '\'');
    }
}


bool Lexer::isValidIdentifierLetter() {
    return ('A' <= *cit && *cit <= 'Z') || ('a' <= *cit && *cit <= 'z') || *cit == '_';
}

char Lexer::peekChar() {
    if (isEnd())
        return '\0';
    auto next_it = cit + 1;
    if (next_it == source.cend())
        return '\0';
    return *cit;
}
