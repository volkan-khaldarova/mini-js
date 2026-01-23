/**
 * @file scanner.c
 * @brief Implementation of the lexical analyzer.
 *
 * Contains the logic for character consumption, pattern matching, and token generation.
 * Optimized for performance with inline functions and branchless logic where applicable.
 */

#include "scanner.h"

// --- THE SCANNER ---

/**
 * @brief Initializes the scanner state.
 *
 * Prepares the scanner to process the given source string. Handles NULL inputs safely.
 *
 * @param scanner Pointer to the scanner structure.
 * @param source The source code string to scan.
 */
void initScanner(Scanner* scanner, const char* source) {
    if (source == NULL) {
        source = "";
    }

    scanner->start = source;
    scanner->current = source;
    scanner->line = 1;
}

/**
 * Checks if the scanner has reached the end of the source string.
 *
 * @param scanner The scanner instance.
 * @return 1 if at end, 0 otherwise.
 * @complexity O(1)
 */
static inline int isAtEnd(Scanner* scanner) {
    return *scanner->current == '\0';
}

/**
 * Consumes the current character and advances the pointer.
 *
 * @param scanner The scanner instance.
 * @return The character that was consumed.
 * @complexity O(1)
 */
static inline char advance(Scanner* scanner) {
    scanner->current++;
    return scanner->current[-1];
}

/**
 * Returns the current character without consuming it (Lookahead 1).
 *
 * @param scanner The scanner instance.
 * @return The current character or '\0' if at end.
 * @complexity O(1)
 */
static inline char peek(Scanner* scanner) {
    if (isAtEnd(scanner)) return '\0';
    return *scanner->current;
}

/**
 * Returns the next character without consuming it (Lookahead 2).
 *
 * Used for detecting tokens like '..' vs number decimals.
 *
 * @param scanner The scanner instance.
 * @return The next character.
 * @complexity O(1)
 */
static inline char peekNext(Scanner* scanner) {
    if (isAtEnd(scanner)) return '\0';
    return scanner->current[1]; 
}

/**
 * Consumes the current character only if it matches the expected one.
 *
 * @param scanner The scanner instance.
 * @param expected The character to match.
 * @return 1 if matched, 0 otherwise.
 * @complexity O(1)
 */
static inline int match(Scanner* scanner, char expected) {
    if (isAtEnd(scanner)) return 0;
    if (*scanner->current != expected) return 0;
    scanner->current++;
    return 1;
}

/**
 * Creates a token from the currently scanned characters.
 *
 * @param scanner The scanner instance.
 * @param type The type of the token to create.
 * @return A new Token struct populated with location and type.
 */
static Token makeToken(Scanner* scanner, TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner->start;
    token.length = (size_t)(scanner->current - scanner->start);
    token.line = scanner->line;
    return token;
}

/**
 * Creates an error token with a message.
 *
 * @param scanner The scanner instance.
 * @param message The error message string.
 * @return A Token of type TOKEN_ERROR containing the message.
 */
static Token errorToken(Scanner* scanner, const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = strlen(message);
    token.line = scanner->line;
    return token;
}

/**
 * @brief Checks if a character is a digit (0-9).
 *
 * OPTIMIZATION: Branchless comparison using unsigned arithmetic.
 * `(c - '0') < 10` is true only for '0'..'9'.
 * Reduces 2 branches to 1 subtraction and 1 comparison.
 */
static inline int isDigit(char c) {
    return (unsigned)(c - '0') < 10;
}

/**
 * @brief Checks if a character is an alphabetic letter or underscore.
 *
 * OPTIMIZATION: Bitwise trick for case-insensitive check.
 * `(c | 32)` converts uppercase to lowercase.
 * Reduces multiple comparisons to simple arithmetic.
 */
static inline int isAlpha(char c) {
    return ((unsigned)((c | 32) - 'a') < 26) || (c == '_');
}

/**
 * @brief Checks if a character is alphanumeric (letter, digit, or underscore).
 */
static inline int isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

/**
 * Checks if the current identifier matches a keyword using a Trie-like strategy.
 *
 * Verifies the remaining characters of a potential keyword after the initial
 * character switch.
 *
 * @param scanner The scanner instance.
 * @param token_length The total length of the current identifier token.
 * @param offset The offset from the start of the token to begin comparison.
 * @param length The length of the remaining string to compare.
 * @param rest The expected remaining string of the keyword.
 * @param type The TokenType to return if the match is successful.
 * @return The matching keyword TokenType, or TOKEN_IDENTIFIER if no match.
 * @complexity O(N) where N is the length of the keyword suffix.
 */
static TokenType checkKeyword(Scanner* scanner, size_t token_length, int offset, size_t length, const char* rest, TokenType type) {
    /* OPTIMIZATION: Use pre-calculated token_length to fail fast without subtraction */
    if (token_length == offset + length && 
        memcmp(scanner->start + offset, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

/**
 * Determines if the scanned identifier is a reserved keyword or a user variable.
 *
 * Uses a hardcoded trie (switch statements) on the first character to quickly
 * narrow down potential keywords.
 *
 * @param scanner The scanner instance.
 * @return The specific keyword TokenType or TOKEN_IDENTIFIER.
 */
static TokenType identifierType(Scanner* scanner) {
    /* OPTIMIZATION: Calculate length once (Hoist out of the switch) */
    size_t len = (size_t)(scanner->current - scanner->start);

    switch (scanner->start[0]) {
        case 'a': return checkKeyword(scanner, len, 1, 2, "nd", TOKEN_AND);
        case 'c': 
            if (len > 1) {
                switch (scanner->start[1]) {
                    case 'l': return checkKeyword(scanner, len, 2, 3, "ass", TOKEN_CLASS);
                    case 'o': return checkKeyword(scanner, len, 2, 3, "nst", TOKEN_CONST);
                }
            }
            break;
        case 'e': return checkKeyword(scanner, len, 1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (len > 1) {
                switch (scanner->start[1]) {
                    case 'a': return checkKeyword(scanner, len, 2, 3, "lse", TOKEN_FALSE);
                    case 'o': return checkKeyword(scanner, len, 2, 1, "r", TOKEN_FOR);
                    case 'u': return checkKeyword(scanner, len, 2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 'i': return checkKeyword(scanner, len, 1, 1, "f", TOKEN_IF);
        case 'l': return checkKeyword(scanner, len, 1, 2, "et", TOKEN_LET);
        case 'n': return checkKeyword(scanner, len, 1, 3, "ull", TOKEN_NULL);
        case 'o': return checkKeyword(scanner, len, 1, 1, "r", TOKEN_OR);
        case 'p': return checkKeyword(scanner, len, 1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(scanner, len, 1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(scanner, len, 1, 4, "uper", TOKEN_SUPER);
        case 't':
            if(len > 1) {
                switch(scanner->start[1]) {
                    case 'h': return checkKeyword(scanner, len, 2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(scanner, len, 2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v': return checkKeyword(scanner, len, 1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(scanner, len, 1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

/**
 * Scans an alphanumeric identifier or keyword.
 *
 * @param scanner The scanner instance.
 * @return A token representing the identifier or keyword.
 */
static Token identifier(Scanner* scanner) {
    while (isAlphaNumeric(peek(scanner))) advance(scanner);
    return makeToken(scanner, identifierType(scanner));
}

/**
 * Scans a numeric literal (integer, float, or BigInt).
 *
 * @param scanner The scanner instance.
 * @return A token representing the number.
 */
static Token number(Scanner* scanner) {
    while(isDigit(peek(scanner))) advance(scanner);

    if (peek(scanner) == '.' && isDigit(peekNext(scanner))) {
        advance(scanner);
        while (isDigit(peek(scanner))) advance(scanner);
    } else if (peek(scanner) == 'n') {
        advance(scanner);
        return makeToken(scanner, TOKEN_BIGINT);
    }
    return makeToken(scanner, TOKEN_NUMBER);
}

/**
 * Scans a string literal enclosed in double quotes.
 *
 * @param scanner The scanner instance.
 * @return A token representing the string, or an error token if unterminated.
 */
static Token string(Scanner* scanner) {
    while (peek(scanner) != '"' && !isAtEnd(scanner)) {
        if (peek(scanner) == '\n') scanner->line++;
        advance(scanner);
    }
    if (isAtEnd(scanner)) return errorToken(scanner, "Unterminated string.");
    advance(scanner);
    return makeToken(scanner, TOKEN_STRING);
 }

/**
 * @brief Main scanning function.
 *
 * Skips whitespace and comments, then identifies the next token in the stream.
 *
 * @param scanner The scanner instance.
 * @return The next Token.
 */
Token scanToken(Scanner* scanner) {
    scan_next:
    for(;;) {
        char c = *scanner->current;
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance(scanner);
            break;
        case '\n':
            scanner->line++;
            advance(scanner);
            break;
        default:
            goto start_scanning;
        }
    }

    start_scanning:
    scanner->start = scanner->current;

    if(isAtEnd(scanner)) return makeToken(scanner, TOKEN_EOF);
    
    char c = advance(scanner);

    switch (c)
    {
    case '(': return makeToken(scanner, TOKEN_LEFT_PAREN);
    case ')': return makeToken(scanner, TOKEN_RIGHT_PAREN);
    case '{': return makeToken(scanner, TOKEN_LEFT_BRACE);
    case '}': return makeToken(scanner, TOKEN_RIGHT_BRACE);
    case ';': return makeToken(scanner, TOKEN_SEMICOLON);
    case ',': return makeToken(scanner, TOKEN_COMMA);
    case '.': return makeToken(scanner, TOKEN_DOT);
    case '-': return makeToken(scanner, TOKEN_MINUS);
    case '+': return makeToken(scanner, TOKEN_PLUS);
    case '*': return makeToken(scanner, TOKEN_STAR);
    case '"': return string(scanner);
    case '/': if (match(scanner, '/')) {
        while (peek(scanner) != '\n' && !isAtEnd(scanner)) advance(scanner);
        goto scan_next;
    } else {
        return makeToken(scanner, TOKEN_SLASH);
    }    

    case '!': return makeToken(scanner, match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=': return makeToken(scanner, match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<': return makeToken(scanner, match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>': return makeToken(scanner, match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    
    default:
        if (isDigit(c)) return number(scanner);
        if (isAlpha(c)) return identifier(scanner);
        return errorToken(scanner, "Unexpected character.");
    }
}