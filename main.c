#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- 1. DATA STRUCTURES ---

typedef enum {
    // --- Primitive (Value) Types ---
    JS_UNDEFINED,
    JS_NULL,
    JS_BOOLEAN,
    JS_NUMBER,
    JS_STRING,
    JS_SYMBOL,

    // --- Object (Reference) Types ---
    JS_OBJECT,
    JS_ARRAY,
    JS_DATE,
    JS_MAP,
    JS_SET,
    JS_WEAKMAP,
    JS_WEAKSET,
    JS_FUNCTION
} JSType;

// [CONCEPT] Tagged Union:
// JavaScript's dynamic nature is hidden here. 'type' tells the variable type,
// 'union' shares the same memory space to hold the value of that type.
typedef struct {
    JSType type;

    union
    {
        /* data */
        double number;
        int boolean;

        char* string;
        char* symbol;

        void* object;
        void* function;
    } as;
} JSValue;


// --- AUXILIARY CREATION FUNCTIONS ---

/**
 * Creates a JSValue representing 'undefined'.
 * @return A JSValue with type JS_UNDEFINED.
 */
JSValue createUNDEFINED() {
    JSValue v;
    v.type = JS_UNDEFINED;
    return v;
}

/**
 * Creates a JSValue representing 'null'.
 * @return A JSValue with type JS_NULL.
 */
JSValue createNULL() {
    JSValue v;
    v.type = JS_NULL;
    return v;
}

/**
 * Creates a JSValue representing a boolean.
 * @param val 1 for true, 0 for false.
 * @return A JSValue with type JS_BOOLEAN.
 */
JSValue createBOOLEAN(int val) {
    JSValue v;
    v.type = JS_BOOLEAN;
    v.as.boolean = val;
    return v;
}

/**
 * Creates a JSValue representing a number.
 * @param val The double precision floating point value.
 * @return A JSValue with type JS_NUMBER.
 */
JSValue createNUMBER(double val) {
    JSValue v;
    v.type = JS_NUMBER;
    v.as.number = val;
    return v;
}

/**
 * Creates a JSValue representing a string.
 * Duplicates the input string using strdup.
 * @param val The C-string to wrap.
 * @return A JSValue with type JS_STRING.
 */
JSValue createSTRING(char* val) {
    JSValue v;
    v.type = JS_STRING;
    v.as.string = strdup(val);
    return v;
}

/**
 * Prints the value of a JSValue to the standard output.
 * Similar to console.log in JS.
 * 
 * @param v The JSValue to be printed.
 */
void printValue(JSValue v) {
    switch (v.type)
    {
    case JS_UNDEFINED:
        /* code */
        printf("undefined");
        break;

    case JS_NULL:
        printf("null");
        break;

    case JS_BOOLEAN:
        if (v.as.boolean == 1) {
            printf("true");
        } else {
            printf("false");
        }
        break;
    
    case JS_NUMBER:
        printf("%g", v.as.number);
        break;
    
    case JS_STRING:
        printf("'%s'", v.as.string);
        break;
    
    case JS_OBJECT:
        printf("[Object]");
        break;

    case JS_ARRAY:
        printf("[Array]");
        break;

    case JS_FUNCTION:
        printf("[Function]");
        break;
    
    default:
        printf("Unknown Type!");
        break;
    }
    printf("\n");
}

/**
 * Frees the memory allocated for a JSValue.
 * Handles manual garbage collection for dynamic types like String.
 * @param v The JSValue to free.
 */
void freeValue(JSValue v) {
    if (v.type == JS_STRING) {
        free(v.as.string);
    }
}

// --- LEXER (TOKEN) STRUCTURES ---
typedef enum {

    // 1. Single Character Tokens
    TOKEN_LEFT_PAREN,  // (
    TOKEN_RIGHT_PAREN, // )
    TOKEN_LEFT_BRACE,  // {
    TOKEN_RIGHT_BRACE, // }
    TOKEN_COMMA,       // ,
    TOKEN_DOT,         // .
    TOKEN_MINUS,       // -
    TOKEN_PLUS,        // +
    TOKEN_SEMICOLON,   // ;
    TOKEN_SLASH,        // /
    TOKEN_STAR,        // *

    // 2. One or two characters
    TOKEN_BANG,          // !
    TOKEN_BANG_EQUAL,    // !=
    TOKEN_EQUAL,         // =
    TOKEN_EQUAL_EQUAL,   // ==
    TOKEN_GREATER,       // >
    TOKEN_GREATER_EQUAL, // >=
    TOKEN_LESS,          // <
    TOKEN_LESS_EQUAL,    // <=
    
    // 3. Literals
    TOKEN_IDENTIFIER,    // Variable names (age, name, x...)
    TOKEN_STRING,        // "Hello World!"
    TOKEN_NUMBER,        // 123

    // 4. Keywords
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE, TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NULL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS, TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    // 5. Special
    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char* start; 
    int length;
    int line;
} Token;

// --- THE SCANNER ---
// [MECHANISM] Two Pointers:
// 'start' points to the beginning of the current lexeme, 'current' moves forward to find the end.
typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

// We create a global scanner instance
Scanner scanner;

/**
 * Initializes the scanner with the source code.
 * Sets the start and current pointers to the beginning.
 * @param source The source code string (must be null-terminated).
 */
void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

/**
 * Checks if the scanner has reached the end of the source code.
 * @return 1 if at end (null terminator), 0 otherwise.
 */
int isAtEnd() {
    return *scanner.current == '\0';
}

/**
 * Consumes the current character and advances the scanner pointer.
 * @return The character that was consumed.
 */
char advance() {
    scanner.current++;
    return scanner.current[-1];
}

/**
 * Returns the current character without consuming it.
 * Used for lookahead.
 * @return The current character.
 */
char peek() {
    if (isAtEnd()) return '\0';
    return *scanner.current;
}

/**
 * Consumes the current character only if it matches the expected one.
 * Used for two-character tokens like '!=' or '=='.
 * @param expected The character to match.
 * @return 1 if matched and consumed, 0 otherwise.
 */
int match(char expected) {
    if (isAtEnd()) return 0;
    if (*scanner.current != expected) return 0;
    scanner.current++;
    return 1;
}

/**
 * Returns the character after the current one without consuming.
 * Used for 2-character lookahead (e.g. detecting numbers with decimals).
 * @return The next character.
 */
char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1]; 
}

/**
 * Checks if a character is a digit (0-9).
 * @param c The character to check.
 * @return 1 if digit, 0 otherwise.
 */
int isDigit(char c) {
    return c >= '0' && c <= '9';
}

/**
 * Checks if a character is an alphabet letter or underscore.
 * @param c The character to check.
 * @return 1 if alpha, 0 otherwise.
 */
int isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

/**
 * Checks if a character is alphanumeric (letter, digit, or underscore).
 * @param c The character to check.
 * @return 1 if alphanumeric, 0 otherwise.
 */
int isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

/**
 * Creates a Token structure based on the current scanner state.
 * Calculates length automatically using pointer arithmetic.
 * 
 * @param type The TokenType (e.g., TOKEN_NUMBER, TOKEN_PLUS).
 */
Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

/**
 * Creates a special token representing a syntax error.
 * @param message The error message to store in the token.
 * @return A Token with type TOKEN_ERROR.
 */
Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

/**
 * Scans a string literal enclosed in double quotes.
 * Handles multi-line strings by updating the line counter.
 * @return A Token with type TOKEN_STRING.
 */
Token string() {

    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    advance();

    return makeToken(TOKEN_STRING);
 }

/**
 * Scans a number literal (integer or floating point).
 * @return A Token with type TOKEN_NUMBER.
 */
Token number() {
    while(isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();

        while (isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
}

/**
 * Checks if the current identifier matches a specific keyword.
 * [LOGIC] Uses a Trie-like check (checking rest of string) for efficiency.
 * @param start Offset from token start.
 * @param length Length of remaining string.
 * @param rest Expected remaining string.
 * @param type Token type to return if matched.
 * @return The keyword TokenType or TOKEN_IDENTIFIER.
 */
TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (scanner.current - scanner.start == start + length && 
        memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

/**
 * Determines the type of the identifier (keyword or variable name).
 * @return The specific keyword TokenType or TOKEN_IDENTIFIER.
 */
TokenType identifierType() {

    switch (scanner.start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
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
 * Scans an identifier or keyword.
 * @return A Token representing the identifier/keyword.
 */
Token identifier() {
    while (isAlphaNumeric(peek())) advance();
    
    return makeToken(identifierType());
}

/**
 * Scans the next token from the source code.
 * Skips whitespace and comments automatically.
 * @return The next Token found.
 */
Token scanToken() {
    // 1. Skip Whitespace (Spaces, tabs, newlines)
    // We loop until we find a real character
    // [STEP 1] Skip Unnecessary Characters:
    // Spaces, tabs, and newlines do not affect the logic, we consume them.
    for(;;) {
        char c = *scanner.current;
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance(); // Just ignore it
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

    // [STEP 2] Token Capture:
    // We set 'start' to the current position to select the new lexeme from here.
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
    // Comment check: If // go until the end of the line.
    case '/': if (match('/')) {
        while (peek() != '\n' && !isAtEnd()) advance();
        return scanToken();
    } else {
        return makeToken(TOKEN_SLASH);
    }    

    // Two-character tokens
    // [STEP 3] Two-Character Check (like !=, ==):
    case '!':
        return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
        return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
        return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
        return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    
    default:

        // [STEP 4] Literals (Numbers and Words):
        if (isDigit(c)) {
            return number();
        } else if (isAlpha(c)) {
            return identifier();
        }

        return errorToken("Unexpected character.");
    }
}


int main() {
    char* source = "var isim = \"MiniJS\";\n"
                   "var yas = 25;\n"
                   "if (yas >= 18) {\n"
                   "  print true;\n"
                   "} else {\n"
                   "  print false;\n"
                   "}";
    
    printf("Scanning Code:\n%s\n", source);
    printf("-------------------------\n");

    initScanner(source);

    int line = -1;
    for (;;) {
        Token token = scanToken();
        
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }

        printf("Type: %2d, Text: '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF) break;
    }

    return 0;
}