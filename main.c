/**
 * @file main.c
 * @brief Entry point for the MiniJS interpreter / Aura language test harness.
 *
 * Currently serves as a test driver to verify the functionality of the Value system
 * (specifically BigInt and Tensors) and the Scanner (Lexer) correctness.
 */

#include "common.h"
#include "scanner.h"
#include "value.h"

/**
 * @brief Main execution entry point.
 *
 * Runs a sequence of tests:
 * 1. Creates and prints a BigInt value.
 * 2. Scans a sample source string containing modern JS/Aura syntax and prints tokens.
 *
 * @return 0 on successful execution.
 */
int main() {
    // --- 1. Value Module Test (BigInt) ---
    printf("=== 1. VALUE TEST (BigInt) ===\n");
    AuraValue myBigInt = createBIGINT(1234567890123456789LL);
    printf("BigInt created in C: ");
    printValue(myBigInt);
    printf("\n");

    // --- 2. Scanner Module Test (let, const, 123n) ---
    printf("=== 2. SCANNER TEST (Modern JS) ===\n");
    char* source = "// Variable types and BigInt test\n"
                   "var x = 10;\n"
                   "let y = 20;\n"
                   "const PI = 3.14159;\n"
                   "var bigNumber = 9876543210987654321n;";
    
    printf("Scanning Code:\n%s\n", source);
    printf("-------------------------\n");

    Scanner scanner;
    initScanner(&scanner, source);

    int line = -1;
    for (;;) {
        Token token = scanToken(&scanner);
        
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }

        printf("Type: %2d, Text: '%.*s'\n", token.type, (int)token.length, token.start);

        if (token.type == TOKEN_EOF) break;
    }

    return 0;
}