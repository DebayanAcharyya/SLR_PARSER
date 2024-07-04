#ifndef TOKENS_H
#define TOKENS_H
    #include<string>
    using namespace std;
    // Token types
    enum TokenType {
        KEYWORD,
        IDENTIFIER,
        DELIMITER,
        OPERATOR,
        INTEGER_LITERAL,
        CHAR_LITERAL,
        STRING_LITERAL,
        INVALID
    };

    // Token structure
    struct Token {
        TokenType type;
        string value;
        int line;
        int position;
    };
#endif