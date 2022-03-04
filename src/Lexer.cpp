//
// Created by mind on 2022/2/25.
//

#include "Lexer.h"

#include <sstream>
#include <cassert>

using namespace CCC;

bool Lexer::isEnd() {
    return this->cit == this->source.cend();
}

void Lexer::getNextToken() {
//    auto &cit=this->cit;    // not essential

    /// skip the whitespace
    while (cit != this->source.cend() && isspace(*cit)) {
        if (*cit == '\n') {
            this->line_num++;
            this->line_head = cit;
            this->line_head++;
        }
        cit++;
    }

    TokenType type;
    TokenLocation location{this->line_num, std::distance(this->line_head, this->cit)};

    int value = 0;
    const auto start = cit;

    if (cit == this->source.cend()) { type = TokenType::Eof; }
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
    } else if (*cit == ';') {
        cit++;
        type = TokenType::Semicolon;
    } else if (*cit == '=') {
        cit++;
        if (this->peekChar() == '=') {
            type = TokenType::EQ;
            cit++;
        } else {
            type = TokenType::Assignment;
        }
    } else if (*cit == '!') {
        cit++;
        if (this->peekChar() == '=') {
            type = TokenType::NE;
            cit++;
        } else {
            diagnose(this->source, this->line_head, location.line_num, location.col_num, this->p_token->content.size(),
                     "unexpected '", *cit, '\'');
        }
    } else if (*cit == '>') {
        cit++;
        if (this->peekChar() == '=') {
            type = TokenType::GE;
            cit++;
        } else {
            type = TokenType::GT;
        }
    } else if (*cit == '<') {
        cit++;
        if (this->peekChar() == '=') {
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
    } else if (this->isValidIdentifierLetter()) {
        type = TokenType::Identifier;
        do {
            cit++;
        } while (!isEnd() && (this->isValidIdentifierLetter() || isdigit(*cit)));
    } else {
//        const auto&[line, coordinates] = locateLine(*this);
//        const auto[line_start, _] = coordinates;
//
//        diagnose(
//                "source code:", location.line_num + 1, ':', location.col_num + 1, ": \033[1;31mError: \033[0m",
//                "unexpected '", *cit, "'\n",
//                '\t', location.line_num, " |\t",
//                line.substr(line_start, location.col_num),
//                "\033[34m", line.at(location.col_num), "\033[0m",
//                line.substr(location.col_num + 1)
//        );
        diagnose(this->source, this->line_head, location.line_num, location.col_num, this->p_token->content.size(),
                 "unexpected '", *cit, '\'');
    }

    this->p_token = std::make_shared<Token>(
            type,
            value,
            this->source.substr(
                    std::distance(this->source.cbegin(), start),
                    std::distance(start, cit)
            ),
            location
    );
}

void Lexer::expectToken(TokenType type) {
    if (this->p_token->type != type) {
        char expected{'\0'};
        switch (type) {
            case TokenType::Semicolon:
                expected = ';';
                break;
            case TokenType::RParenthesis:
                expected = ')';
                break;
            default:
                assert(1);
        }
        const auto&[line_num, col_num]=this->p_token->location;
        diagnose(this->source, this->line_head, line_num, col_num, 0,
                 "expected '", expected, '\'');
    }
}


bool Lexer::isValidIdentifierLetter() {
    return ('A' <= *cit && *cit <= 'Z') || ('a' <= *cit && *cit <= 'z') || *cit == '_';
}

char Lexer::peekChar() {
    if (this->isEnd())
        return '\0';
    auto next_it = this->cit + 1;
    if (next_it == this->source.cend())
        return '\0';
    return *cit;
}
