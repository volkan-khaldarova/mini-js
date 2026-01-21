#include "common.h"
#include "scanner.h"

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