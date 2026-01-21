#include "common.h"
#include "scanner.h"
#include "value.h"

/**
 * @brief Entry point for the MiniJS interpreter.
 * Currently runs tests for the Value system and the Scanner (Lexer).
 */
int main() {
    // --- 1. Value Modulu Testi (BigInt) ---
    printf("=== 1. VALUE TEST (BigInt) ===\n");
    JSValue myBigInt = createBIGINT(1234567890123456789LL);
    printf("C tarafinda olusturulan BigInt: ");
    printValue(myBigInt);
    printf("\n");

    // --- 2. Scanner Modulu Testi (let, const, 123n) ---
    printf("=== 2. SCANNER TEST (Modern JS) ===\n");
    char* source = "// Degisken tipleri ve BigInt testi\n"
                   "var x = 10;\n"
                   "let y = 20;\n"
                   "const PI = 3.14159;\n"
                   "var buyukSayi = 9876543210987654321n;";
    
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