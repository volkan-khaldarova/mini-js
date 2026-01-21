#include "scanner.h"

// --- THE SCANNER ---
/**
 * Internal state of the scanner.
 * Tracks the current position in the source code.
 */
typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

/**
 * Checks if the scanner has reached the end of the source string.
 * @return 1 if at end, 0 otherwise.
 */
static int isAtEnd() {
    return *scanner.current == '\0';
}

/**
 * Consumes the current character and advances the pointer.
 * @return The character that was consumed.
 */
static char advance() {
    scanner.current++;
    return scanner.current[-1];
}

/**
 * Returns the current character without consuming it (Lookahead 1).
 * @return The current character or '\0' if at end.
 */
static char peek() {
    if (isAtEnd()) return '\0';
    return *scanner.current;
}

/**
 * Returns the next character without consuming it (Lookahead 2).
 * Used for detecting tokens like '..' vs number decimals.
 * @return The next character.
 */
static char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1]; 
}

/**
 * Consumes the current character only if it matches the expected one.
 * @return 1 if matched, 0 otherwise.
 */
static int match(char expected) {
    if (isAtEnd()) return 0;
    if (*scanner.current != expected) return 0;
    scanner.current++;
    return 1;
}

/**
 * Creates a token from the currently scanned characters.
 */
static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

/**
 * Creates an error token with a message.
 */
static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

static int isDigit(char c) {
    return c >= '0' && c <= '9';
}

static int isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static int isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

/**
 * Checks if the current identifier matches a keyword using a Trie-like strategy.
 * Verifies the remaining characters of a potential keyword.
 */
static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length && 
        memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

/**
 * Determines if the scanned identifier is a reserved keyword or a user variable.
 */
static TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': 
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'l': return checkKeyword(2, 3, "ass", TOKEN_CLASS);
                    case 'o': return checkKeyword(2, 3, "nst", TOKEN_CONST);
                }
            }
            break;
        case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                    case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
        case 'l': return checkKeyword(1, 2, "et", TOKEN_LET);
        case 'n': return checkKeyword(1, 3, "ull", TOKEN_NULL);
        case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if(scanner.current - scanner.start > 1) {
                switch(scanner.start[1]) {
                    case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

/**
 * Scans an alphanumeric identifier or keyword.
 */
static Token identifier() {
    while (isAlphaNumeric(peek())) advance();
    return makeToken(identifierType());
}

/**
 * Scans a numeric literal (integer, float, or BigInt).
 */
static Token number() {
    while(isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    } else if (peek() == 'n') {
        advance();
        return makeToken(TOKEN_BIGINT);
    }
    return makeToken(TOKEN_NUMBER);
}

/**
 * Scans a string literal enclosed in double quotes.
 */
static Token string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }
    if (isAtEnd()) return errorToken("Unterminated string.");
    advance();
    return makeToken(TOKEN_STRING);
 }

Token scanToken() {
    for(;;) {
        char c = *scanner.current;
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            scanner.line++;
            advance();
            break;
        default:
            goto start_scanning;
        }
    }

    start_scanning:
    scanner.start = scanner.current;

    if(isAtEnd()) return makeToken(TOKEN_EOF);
    
    char c = advance();

    switch (c)
    {
    case '(': return makeToken(TOKEN_LEFT_PAREN);
    case ')': return makeToken(TOKEN_RIGHT_PAREN);
    case '{': return makeToken(TOKEN_LEFT_BRACE);
    case '}': return makeToken(TOKEN_RIGHT_BRACE);
    case ';': return makeToken(TOKEN_SEMICOLON);
    case ',': return makeToken(TOKEN_COMMA);
    case '.': return makeToken(TOKEN_DOT);
    case '-': return makeToken(TOKEN_MINUS);
    case '+': return makeToken(TOKEN_PLUS);
    case '*': return makeToken(TOKEN_STAR);
    case '"': return string();
    case '/': if (match('/')) {
        while (peek() != '\n' && !isAtEnd()) advance();
        goto start_scanning;
    } else {
        return makeToken(TOKEN_SLASH);
    }    

    case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=': return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<': return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    
    default:
        if (isDigit(c)) return number();
        if (isAlpha(c)) return identifier();
        return errorToken("Unexpected character.");
    }
}