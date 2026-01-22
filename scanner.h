#ifndef minijs_scanner_h
#define minijs_scanner_h

/**
 * @file scanner.h
 * @brief Defines the lexical analyzer (scanner) for the Aura language.
 *
 * The scanner is responsible for converting raw source code strings into a stream
 * of tokens. It handles keyword recognition, literal parsing, and basic error detection.
 */

#include "common.h"

/**
 * @brief Enumeration of all possible token types in the language.
 *
 * Categorizes tokens into single-character punctuation, operators, literals,
 * keywords, and special control tokens.
 */
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
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, TOKEN_BIGINT,

    // 4. Keywords
    TOKEN_AND, TOKEN_CLASS, TOKEN_CONST, TOKEN_ELSE, TOKEN_FALSE, 
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_LET, TOKEN_NULL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS, 
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    // 5. Special
    TOKEN_ERROR, TOKEN_EOF
} TokenType;

/**
 * @brief Represents a single unit of code (lexeme).
 *
 * A token views a slice of the source code. It does not own the string data;
 * it points to the start in the source string and stores the length.
 */
typedef struct {
    TokenType type;
    const char* start; 
    size_t length;
    int line;
} Token;

/**
 * @brief Internal state of the scanner.
 *
 * Maintains the current position in the source string and line tracking.
 * Exposed in the header to allow stack allocation, enabling thread safety
 * and reentrancy (multiple scanners running in parallel).
 */
typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

/**
 * Initializes the scanner with the source code.
 *
 * Sets up the scanner state to begin processing the provided source string.
 *
 * @param scanner Pointer to the scanner instance.
 * @param source The null-terminated source code string.
 */
void initScanner(Scanner* scanner, const char* source);

/**
 * Scans the next token from the source code.
 *
 * Advances the scanner through the source code until a complete token is formed
 * or an error is encountered.
 *
 * @param scanner Pointer to the scanner instance.
 * @return The next Token found in the stream.
 * @complexity O(L) where L is the length of the token (amortized O(1) relative to source length).
 */
Token scanToken(Scanner* scanner);

#endif