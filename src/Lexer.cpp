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
    while (this->cit != this->source.cend() && isspace(*cit)) {
        cit++;
    }

    TokenType type;
    int value = 0;
    const auto start = cit;
    std::stringstream ss;

    if (cit == this->source.cend()) { type = TokenType::Eof; }
    else if (*cit == '+') {
        type = TokenType::Add;
        cit++;
    } else if (*cit == '-') {
        type = TokenType::Sub;
        cit++;
    } else if (*cit == '*') {
        type = TokenType::Mul;
        cit++;
    } else if (*cit == '/') {
        type = TokenType::Div;
        cit++;
    } else if (*cit == '(') {
        type = TokenType::LParenthesis;
        cit++;
    } else if (*cit == ')') {
        type = TokenType::RParenthesis;
        cit++;
    } else if (*cit == ';') {
        type = TokenType::Semicolon;
        cit++;
    } else if (*cit == '=') {
        type = TokenType::Assignment;
        cit++;
    } else if (isdigit(*cit)) {
        type = TokenType::Num;
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
        assert(0);
    }

    this->p_token = std::make_shared<Token>();
    this->p_token->type = type;
    this->p_token->value = value;
    this->p_token->content = this->source.substr(
            std::distance(this->source.cbegin(), start),
            std::distance(start, cit)
    );
}

bool Lexer::isValidIdentifierLetter() {
    return ('A' <= *cit && *cit <= 'Z') || ('a' <= *cit && *cit <= 'z') || *cit == '_';
}




