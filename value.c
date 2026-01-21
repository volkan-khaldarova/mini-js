#include "value.h"

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
 * Duplicates the input string using strdup and checks for allocation failure.
 * @param val The C-string to wrap.
 * @return A JSValue with type JS_STRING.
 */
JSValue createSTRING(char* val) {
   JSValue v;
    v.type = JS_STRING;
    v.as.string = strdup(val);
    
    if (v.as.string == NULL) {
        fprintf(stderr, "[Fatal Error] Out of memory in createSTRING.\n");
        exit(1);
    }

    return v;
}

/**
 * Creates a JSValue representing a BigInt.
 * @param val The long long integer value.
 * @return A JSValue with type JS_BIGINT.
 */
JSValue createBIGINT(long long val) {
    JSValue v;
    v.type = JS_BIGINT;
    v.as.bigint = val;
    return v;
}

/**
 * Prints the value of a JSValue to the standard output.
 * Handles formatting for different types (e.g., 'n' suffix for BigInt).
 */
void printValue(JSValue v) {
    switch (v.type)
    {
    case JS_UNDEFINED:
        printf("undefined");
        break;
    case JS_NULL:
        printf("null");
        break;
    case JS_BOOLEAN:
        printf(v.as.boolean ? "true" : "false");
        break;
    case JS_NUMBER:
        printf("%g", v.as.number);
        break;
    case JS_STRING:
        printf("'%s'", v.as.string);
        break;
    case JS_BIGINT:
        printf("%lldn", v.as.bigint);
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
 */
void freeValue(JSValue v) {
    if (v.type == JS_STRING) {
        free(v.as.string);
    }
}