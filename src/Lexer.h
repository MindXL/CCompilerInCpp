//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_LEXER_H
#define CCOMPILERINCPP_LEXER_H

#include <string_view>
#include <memory>

#include "diagnose.h"

namespace CCC {

    enum class TokenType {
        Add, Sub, Mul, Div,
        Num,
        LParenthesis, RParenthesis,
        Identifier,
        Semicolon,
        Assignment,
        EQ, NE, GT, GE, LT, LE,
        Eof
    };

    struct TokenLocation {
        // 每一个token的位置信息
        const long line_num;
        const long col_num;

        TokenLocation(long line_num, long col_num) : line_num{line_num}, col_num{col_num} {}

        TokenLocation(TokenLocation &location) = default;
    };

    class Token {
    public:
        TokenType type;
        int value;
        std::string_view content;
        TokenLocation location;

        Token(TokenType type, int value, std::string_view &&content, TokenLocation &location) :
                type{type}, value{value}, content{content}, location{location} {}
    };

    class Lexer {
    public:
        std::string_view source;
        std::shared_ptr<Token> p_token;

    private:
        std::string_view::const_iterator cit;

    public:
        int line_num{0};
        std::string_view::const_iterator line_head;

    public:
        explicit Lexer(const char *code) : source{code}, cit{source.cbegin()}, line_head{source.cbegin()} {}

        bool isEnd();

        void getNextToken();

        void expectToken(TokenType type);

    private:
        bool isValidIdentifierLetter();

        char peekChar();
    };
}


#endif //CCOMPILERINCPP_LEXER_H
