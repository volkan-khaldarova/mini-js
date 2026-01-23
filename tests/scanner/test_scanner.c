#include "../../tests/unity/unity.h"
#include "scanner.h"

/**
 * @file test_scanner.c
 * @brief Unit tests for the Aura lexical analyzer.
 *
 * This suite verifies the correctness of the scanner by checking token generation
 * for various language constructs including punctuation, operators, literals,
 * keywords, and comments.
 */

Scanner scanner;

/**
 * @brief Test setup hook.
 *
 * Executed before each test case. Currently a no-op as the scanner
 * is initialized explicitly within each test function.
 */
void setUp(void) {
}

/**
 * @brief Test teardown hook.
 *
 * Executed after each test case.
 */
void tearDown(void) {
}

/**
 * @brief Verifies that the next token matches the expected type and text.
 *
 * Acts as a custom assertion helper to reduce boilerplate in test cases.
 * It advances the scanner state by one token.
 *
 * @param expected_type The expected classification of the token (TokenType).
 * @param expected_text The expected substring from the source code.
 */
void assert_token(TokenType expected_type, const char* expected_text) {
    Token token = scanToken(&scanner);
    
    // Verify the token classification
    TEST_ASSERT_EQUAL_INT(expected_type, token.type);
    
    // Verify the lexeme content
    if (expected_text != NULL) {
        char buffer[1024];
        
        // Tokens are views into the source string and are not null-terminated.
        // We must copy the data to a buffer to perform a string comparison.
        memcpy(buffer, token.start, token.length);
        buffer[token.length] = '\0';
        
        TEST_ASSERT_EQUAL_STRING(expected_text, buffer);
    }
}

// --- TEST CASES ---

/**
 * @brief Tests single-character punctuation and grouping symbols.
 *
 * Verifies parsing of parentheses, braces, semicolons, and commas.
 */
void test_punctuation(void) {
    initScanner(&scanner, "(){};,");
    
    assert_token(TOKEN_LEFT_PAREN, "(");
    assert_token(TOKEN_RIGHT_PAREN, ")");
    assert_token(TOKEN_LEFT_BRACE, "{");
    assert_token(TOKEN_RIGHT_BRACE, "}");
    assert_token(TOKEN_SEMICOLON, ";");
    assert_token(TOKEN_COMMA, ",");
    assert_token(TOKEN_EOF, "");
}

/**
 * @brief Tests operator recognition.
 *
 * Specifically checks the scanner's ability to distinguish between
 * single-character operators (e.g., '!') and two-character operators (e.g., '!=').
 */
void test_operators(void) {
    initScanner(&scanner, "! !=");
    
    assert_token(TOKEN_BANG, "!");
    assert_token(TOKEN_BANG_EQUAL, "!=");
    assert_token(TOKEN_EOF, "");
}

/**
 * @brief Tests string literal parsing.
 *
 * Ensures that string tokens include the surrounding quotes as per the
 * scanner's design.
 */
void test_string_literals(void) {
    initScanner(&scanner, "\"merhaba\"");
    
    // The scanner implementation includes quotes in the token lexeme.
    assert_token(TOKEN_STRING, "\"merhaba\""); 
}

/**
 * @brief Tests numeric literal parsing.
 *
 * Verifies correct identification of integers and floating-point numbers.
 */
void test_numeric_literals(void) {
    initScanner(&scanner, "123 45.67");
    
    assert_token(TOKEN_NUMBER, "123");
    assert_token(TOKEN_NUMBER, "45.67");
}

/**
 * @brief Tests keyword vs. identifier resolution.
 *
 * Verifies that reserved words (e.g., 'var', 'return') are correctly
 * identified as keywords, while similar strings are treated as identifiers.
 * This exercises the Trie/Switch logic in the scanner.
 */
void test_keywords_and_identifiers(void) {
    initScanner(&scanner, "var variable return");
    
    assert_token(TOKEN_VAR, "var");
    assert_token(TOKEN_IDENTIFIER, "variable");
    assert_token(TOKEN_RETURN, "return");
}

/**
 * @brief Tests comment skipping logic.
 *
 * Ensures that single-line comments are ignored and do not produce tokens.
 */
void test_ignore_comments(void) {
    initScanner(&scanner, "// bu bir yorum \n var");
    
    // The comment should be consumed entirely, yielding the next valid token.
    assert_token(TOKEN_VAR, "var");
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_punctuation);
    RUN_TEST(test_operators);
    RUN_TEST(test_string_literals);
    RUN_TEST(test_numeric_literals);
    RUN_TEST(test_keywords_and_identifiers);
    RUN_TEST(test_ignore_comments);
    
    return UNITY_END();
}