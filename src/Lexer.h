//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_LEXER_H
#define CCOMPILERINCPP_LEXER_H


#include <string_view>
#include <memory>

namespace CCC {

    enum class TokenType {
        Add, Sub, Mul, Div,
        Num,
        LParenthesis, RParenthesis,
        Identifier,
        Semicolon,
        Assignment,
        Eof
    };

    class Token {
    public:
        TokenType type;
        int value;
        std::string_view content;
    };

    class Lexer {
    private:
        std::string_view source;
        std::string_view::const_iterator cit;
    public:
        std::shared_ptr<Token> p_token;

    public:
        explicit Lexer(const char *code) : source{code}, cit{source.cbegin()} {}

        bool isEnd();

        void getNextToken();

    private:
        bool isValidIdentifierLetter();
    };
}


#endif //CCOMPILERINCPP_LEXER_H
