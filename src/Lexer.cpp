//
// Created by mind on 2022/2/25.
//

#include "Lexer.h"

#include <sstream>
#include <cassert>

using namespace CCC;

inline bool Lexer::isEnd() {
    return this->cit == this->source.cend();
}

void Lexer::getNextToken() {
    /// skip the whitespace
    while (!isEnd() && isspace(*(this->cit))) {
        this->cit++;
    }

    TokenType type;
    int value = 0;
    auto start = this->cit;
    std::stringstream ss;

    if (this->cit == this->source.cend()) { type = TokenType::Eof; }
    else if (*(this->cit) == '+') {
        type = TokenType::Add;
        this->cit++;
    } else if (*(this->cit) == '-') {
        type = TokenType::Sub;
        this->cit++;
    } else if (*(this->cit) == '*') {
        type = TokenType::Mul;
        this->cit++;
    } else if (*(this->cit) == '/') {
        type = TokenType::Div;
        this->cit++;
    } else if (isdigit(*(this->cit))) {
        type = TokenType::Num;
        do {
            ss << *(this->cit);
            this->cit++;
        } while (!isEnd() && isdigit(*(this->cit)));
        ss >> value;
    } else {
        // Error parsing
        assert(0);
    }
    this->p_token = std::make_shared<Token>();
    this->p_token->type = type;
    this->p_token->value = value;
    this->p_token->content = this->source.substr(
            std::distance(this->source.cbegin(), start),
            std::distance(start, this->cit)
    );
}

std::string_view Lexer::TEST_get_cur_content() {
    return this->p_token->content;
}