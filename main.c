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

    // --- Object (Referans) Tipleri ---
    JS_OBJECT,
    JS_ARRAY,
    JS_DATE,
    JS_MAP,
    JS_SET,
    JS_WEAKMAP,
    JS_WEAKSET,
    JS_FUNCTION
} JSType;

typedef struct {
    JSType type;

    union main
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

// 1. Undefined Generator
JSValue createUNDEFINED() {
    JSValue v;
    v.type = JS_UNDEFINED;
    return v;
}

// 2. NULL Generator
JSValue createNULL() {
    JSValue v;
    v.type = JS_NULL;
    return v;
}

// 3. BOOLEAN Oluşturucu
JSValue createBOOLEAN(int val) {
    JSValue v;
    v.type = JS_BOOLEAN;
    v.as.boolean = val;
    return v;
}

// 4. NUMBER Oluşturucu
JSValue createNUMBER(double val) {
    JSValue v;
    v.type = JS_NUMBER;
    v.as.number = val;
    return v;
}

// 5. STRING Generator
JSValue createSTRING(char* val) {
    JSValue v;
    v.type = JS_STRING;
    v.as.string = strdup(val);
    return v;
}

// Function that prints the value to the screen
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

// Function that cleans the memory (Manual Garbage Collection)
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
typedef struct {
    const char* start;
    const char* current;
    int line;
} Scanner;

// We create a global scanner instance
Scanner scanner;

// Function to reset the scanner
void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

// 1. Check if we reached the end of the code
int isAtEnd() {
    return *scanner.current == '\0';
}

// 2. Read the current character and move to the next one
char advance() {
    scanner.current++;
    return scanner.current[-1];
}

// Look ahead but don't consume (Simdilik kullanilmiyor ama string/number icin gerekecek)
char peek() {
    if (isAtEnd()) return '\0';
    return *scanner.current;
}

// Conditional advance (for two-char tokens like !=, ==)
int match(char expected) {
    if (isAtEnd()) return 0;
    if (*scanner.current != expected) return 0;
    scanner.current++;
    return 1;
}

// 3. Create a Token (The "Pointer" logic)
Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

// Create an Error Token
Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

Token scanToken() {
    // 1. Skip Whitespace (Spaces, tabs, newlines)
    // We loop until we find a real character
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
    case '/': return makeToken(TOKEN_SLASH);
    case '*': return makeToken(TOKEN_STAR);

    // Two-character tokens
    case '!':
        return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
        return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
        return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
        return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    
    default:
        return errorToken("Unexpected character.");
    }
}


int main() {
    // Our fake source code
    char* source = "({ + - ; })";
    
    printf("Scanning code: %s\n", source);
    printf("-------------------------\n");

    initScanner(source);

    // Loop until we hit the End of File (EOF)
    int line = -1;
    for (;;) {
        Token token = scanToken();
        
        if (token.line != line) {
            printf("%4d ", token.line); // Print line number only if it changes
            line = token.line;
        } else {
            printf("   | ");
        }

        printf("Token Type: %d, Text: '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF) break;
    }

    return 0;
}