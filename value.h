#ifndef minijs_value_h
#define minijs_value_h

#include "common.h"

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
typedef struct {
    JSType type;

    union
    {
        double number;
        int boolean;
        char* string;
        char* symbol;
        void* object;
        void* function;
    } as;
} JSValue;

// Function Prototypes
JSValue createUNDEFINED();
JSValue createNULL();
JSValue createBOOLEAN(int val);
JSValue createNUMBER(double val);
JSValue createSTRING(char* val);

void printValue(JSValue v);
void freeValue(JSValue v);

#endif
