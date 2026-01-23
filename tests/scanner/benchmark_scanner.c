#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../../include/scanner.h"

/**
 * @file benchmark_scanner.c
 * @brief Performance benchmarking tool for the Aura Scanner.
 *
 * This utility measures the throughput of the lexical analyzer by processing
 * a large, synthetically generated block of source code. It calculates
 * execution time and processing speed in MB/s to help identify regressions
 * or validate optimizations.
 */

/**
 * @brief Generates a large synthetic source code string.
 *
 * Constructs a repetitive string based on a predefined pattern containing
 * keywords, identifiers, numbers, and operators to stress-test the scanner.
 *
 * @param iterations The number of times to repeat the test pattern.
 * @return A pointer to the heap-allocated source string. The caller is responsible for freeing this memory.
 * @complexity O(N) where N is the total length of the generated string.
 */
char* generateLargeSource(int iterations) {
    // A pattern containing a mix of keywords, operators, literals, and identifiers
    // to exercise different paths in the scanner's state machine.
    const char* pattern = "var x = 123.45 + variable; if (true) return null; ";
    size_t pattern_len = strlen(pattern);
    size_t total_len = pattern_len * iterations;

    char* source = (char*)malloc(total_len + 1);
    if (source == NULL) {
        fprintf(stderr, "Fatal: Memory allocation failed for benchmark source.\n");
        exit(1);
    }

    char* ptr = source;
    for (int i = 0; i < iterations; i++) {
        memcpy(ptr, pattern, pattern_len);
        ptr += pattern_len;
    }
    *ptr = '\0';
    
    return source;
}

/**
 * @brief Main entry point for the benchmark.
 *
 * Orchestrates the benchmark lifecycle:
 * 1. Generates the test payload.
 * 2. Initializes the scanner.
 * 3. Measures the wall-clock time required to tokenize the entire payload.
 * 4. Reports performance metrics (Time, MB/s).
 *
 * @return 0 on success.
 */
int main(void) {
    // 1. Setup: Generate a massive source code string (e.g., 100,000 repetitions).
    printf("Generating source code payload...\n");
    int repeat_count = 100000;
    char* source = generateLargeSource(repeat_count);
    size_t source_length = strlen(source);
    
    printf("Starting benchmark...\n");
    printf("Total characters to scan: %zu\n", source_length);

    Scanner scanner;
    initScanner(&scanner, source);

    // 2. Measurement Start
    clock_t start_time = clock();

    // 3. Execution: Consume tokens until EOF is reached.
    // This loop forces the scanner to process the entire string.
    Token token;
    do {
        token = scanToken(&scanner);
    } while (token.type != TOKEN_EOF);

    // 4. Measurement End
    clock_t end_time = clock();

    // 5. Calculation & Reporting
    // Convert clock ticks to seconds.
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("\n--------------------------------\n");
    printf("BENCHMARK COMPLETE\n");
    printf("Elapsed Time: %.4f seconds\n", time_spent);
    
    // Calculate throughput in Megabytes per second (MB/s).
    double speed_mb = (source_length / 1024.0 / 1024.0) / time_spent;
    printf("Throughput:   %.2f MB/s\n", speed_mb);
    printf("--------------------------------\n");

    // Cleanup
    free(source);
    return 0;
}