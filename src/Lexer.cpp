//
// Created by mind on 2022/2/25.
//

#include "Lexer.h"

using namespace CCC;

Lexer::Lexer(const char *filename) : ifs{filename, std::ios::in} {
    if (!ifs.is_open()) {
        std::cerr << "failed to open file " << filename << std::endl;
        exit(1);
    }
}

Lexer::~Lexer() {
    ifs.close();
}

bool Lexer::isEnd() {
    return ifs.eof() && cit == line.cend();
}

void Lexer::getNextToken() {
    /// update new line
    /// skip whitespace
    while (!isEnd() && (*cit == '\0' || cit == line.cend() || isspace(*cit))) {
        next();
    }

    TokenType type;
    const auto start_pos = std::distance(line.cbegin(), cit);
    assert(start_pos >= 0);    // ensure that count from line.cbegin() to cit.
    TokenLocation location{n_line, size_t(start_pos)};

    int value = 0;
    std::string content;

    if (isEnd()) {
        type = TokenType::Eof;
        content = '\0';
    } else if (*cit == '+') {
        next();
        type = TokenType::Add;
        content = '+';
    } else if (*cit == '-') {
        next();
        type = TokenType::Sub;
        content = '-';
    } else if (*cit == '*') {
        next();
        type = TokenType::Mul;
        content = '*';
    } else if (*cit == '/') {
        next();
        type = TokenType::Div;
        content = '/';
    } else if (*cit == '(') {
        next();
        type = TokenType::LParenthesis;
        content = '(';
    } else if (*cit == ')') {
        next();
        type = TokenType::RParenthesis;
        content = ')';
    } else if (*cit == '{') {
        next();
        type = TokenType::LBrace;
        content = '{';
    } else if (*cit == '}') {
        next();
        type = TokenType::RBrace;
        content = '}';
    } else if (*cit == ';') {
        next();
        type = TokenType::Semicolon;
        content = ';';
    } else if (*cit == ',') {
        next();
        type = TokenType::Comma;
        content = ',';
    } else if (*cit == '=') {
        next();
        if (*cit == '=') {
            next();
            type = TokenType::EQ;
            content = "==";
        } else {
            type = TokenType::Assignment;
            content = '=';
        }
    } else if (*cit == '!') {
        next();
        if (*cit == '=') {
            next();
            type = TokenType::NE;
            content = "!=";
        } else {
            // TODO: consider to do recursive getNextToken() and use those token's content as info.
            diagnose(line, location.n_line, location.start_pos + 1, 1,
                     "unexpected '", *std::next(cit), '\'');
        }
    } else if (*cit == '>') {
        next();
        if (*cit == '=') {
            next();
            type = TokenType::GE;
            content = ">=";
        } else {
            type = TokenType::GT;
            content = '>';
        }
    } else if (*cit == '<') {
        next();
        if (*cit == '=') {
            next();
            type = TokenType::LE;
            content = "<=";
        } else {
            type = TokenType::LT;
            content = '<';
        }
    } else if (isdigit(*cit)) {
        type = TokenType::Num;
        std::stringstream ss;
        do {
            ss << *cit;
            content += *cit;
            next();
        } while (!isEnd() && isdigit(*cit));
        ss >> value;
    } else if (isValidIdentifierLetter()) {
        do {
            content += *cit;
            next();
        } while (!isEnd() && (isValidIdentifierLetter() || isdigit(*cit)));

        if (content == "if") {
            type = TokenType::IF;
        } else if (content == "else") {
            type = TokenType::ELSE;
        } else if (content == "while") {
            type = TokenType::WHILE;
        } else if (content == "do") {
            type = TokenType::DO;
        } else if (content == "for") {
            type = TokenType::FOR;
        } else {
            type = TokenType::Identifier;
        }
    } else {
        diagnose(line, location.n_line, location.start_pos, 1,
                 "unexpected '", *cit, '\'');
    }

    p_token = std::make_shared<Token>(type, value, content, location);
}

bool Lexer::expectToken(TokenType type) {
    // This function will pass the current token.
    if (p_token->type != type) {
        char expected{'\0'};
        switch (type) {
            case TokenType::Semicolon:
                expected = ';';
                break;
            case TokenType::LParenthesis:
                expected = '(';
                break;
            case TokenType::RParenthesis:
                expected = ')';
                break;
            case TokenType::RBrace:
                expected = '}';
                break;
            default:
                assert(0);
        }
        const auto &location = p_token->location;
        diagnose(line, location.n_line, location.start_pos, p_token->content.length(),
                 "expected '", expected, '\'');
    }
    getNextToken();
    return true;
}

void Lexer::next() {
    // cit should never been directly altered outside this function.
    if (cit == line.cend()) {
        line.clear();
        for (auto ch = ifs.get(); ch != '\n' && ch != EOF; ch = ifs.get()) {
            line += char(ch);
        }
        line.shrink_to_fit();
        cit = line.cbegin();
        n_line++;
    } else {
        cit++;
    }
}

bool Lexer::isValidIdentifierLetter() {
    return ('A' <= *cit && *cit <= 'Z') || ('a' <= *cit && *cit <= 'z') || *cit == '_';
}
