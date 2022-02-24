//
// Created by mind on 2022/2/25.
//

#include "Lexer.h"
#include<sstream>

constexpr bool CCC::Lexer::isEnd() {
    return this->cit == this->source_code.cend();
}

void CCC::Lexer::getNextToken() {
    /// skip the whitespace
    while (!isEnd() && isspace(*(this->cit))) {
        this->cit++;
    }

    TokenType type;
    int value = 0;
    auto start = this->cit;
    std::stringstream ss;

    if (this->cit == this->source_code.cend()) { type = TokenType::Eof; }
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
//        throw std::exception()
    }
    this->p_token = std::make_shared<Token>();
    this->p_token->type = type;
    this->p_token->value = value;
    this->p_token->content = this->source_code.substr(
            std::distance(this->source_code.cbegin(), start),
            std::distance(start, this->cit)
    );
}

std::string_view CCC::Lexer::TEST_get_cur_content() {
    return this->p_token->content;
}