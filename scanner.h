#ifndef minijs_scanner_h
#define minijs_scanner_h

#include "common.h"

typedef enum {
    // 1. Single Character Tokens
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    // 2. One or two characters
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,
    
    // 3. Literals
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // 4. Keywords
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE, 
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NULL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS, 
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    // 5. Special
    TOKEN_ERROR, TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start; 
    int length;
    int line;
} Token;

/**
 * Initializes the scanner with the source code.
 * @param source The source code string.
 */
void initScanner(const char* source);

/**
 * Scans the next token from the source code.
 * @return The next Token found.
 */
Token scanToken();

#endif