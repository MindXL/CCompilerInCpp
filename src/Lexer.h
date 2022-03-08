//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_LEXER_H
#define CCOMPILERINCPP_LEXER_H

#include <fstream>
#include <iostream>
#include <string>
#include <memory>

#include "diagnose.h"

namespace CCC {

    enum class TokenType {
        Add, Sub, Mul, Div,
        Num,
        LParenthesis, RParenthesis,
        LBrace, RBrace,
        Identifier,
        Semicolon,
        Assignment,
        EQ, NE, GT, GE, LT, LE,

        IF, ELSE,
        WHILE, DO,

        Eof
    };

    struct TokenLocation {
        // 每一个token的位置信息
        const size_t n_line;    // count from 1
        const size_t start_pos;    // count from 0

        TokenLocation(size_t n_line, size_t start_pos) : n_line{n_line}, start_pos{start_pos} {}

        TokenLocation(TokenLocation &location) = default;
    };

    class Token {
    public:
        TokenType type;
        int value;
        std::string content;
        TokenLocation location;

        Token(TokenType type, int value, std::string &content, TokenLocation &location) :
                type{type}, value{value}, content{content}, location{location} {}
    };

    class Lexer {
    private:
        std::ifstream ifs;

    public:
        std::shared_ptr<Token> p_token;
        std::string line;
        size_t n_line{0};    // count from 1

    private:
        std::string::const_iterator cit{line.cbegin()};

    public:
        explicit Lexer(const char *filename);

        ~Lexer();

        bool isEnd();

        void getNextToken();

        void expectToken(TokenType type);

    private:
        void next();

        bool isValidIdentifierLetter();
    };
}


#endif //CCOMPILERINCPP_LEXER_H
