#ifndef minijs_value_h
#define minijs_value_h

#include "common.h"

/**
 * @brief Enumeration of all supported JavaScript data types.
 * Distinguishes between primitive values and reference objects.
 */
typedef enum {
    // --- Primitive (Value) Types ---
    JS_UNDEFINED,
    JS_NULL,
    JS_BOOLEAN,
    JS_NUMBER,
    JS_STRING,
    JS_SYMBOL,
    JS_BIGINT,

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

/**
 * @brief Represents a dynamic JavaScript value.
 * 
 * Uses a "Tagged Union" pattern: 'type' indicates which field of the 'union' is valid.
 */
typedef struct {
    JSType type;

    union
    {
        double number;
        int boolean;
        char* string;
        char* symbol;
        long long bigint;
        void* object;
        void* function;
    } as;
} JSValue;

// --- Value Creation & Management ---
JSValue createUNDEFINED();
JSValue createNULL();
JSValue createBOOLEAN(int val);
JSValue createNUMBER(double val);
JSValue createSTRING(char* val);
JSValue createBIGINT(long long val);

void printValue(JSValue v);
void freeValue(JSValue v);

#endif