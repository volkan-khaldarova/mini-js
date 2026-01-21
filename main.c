#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- 1. DATA STRUCTURES ---

typedef enum {
    // --- Primitive (Value) Types ---
    JS_UNDEFINED,
    JS_NULL,
    JS_BOOLEN,
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
JSValue createBOOLEN(int val) {
    JSValue v;
    v.type = JS_BOOLEN;
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

    case JS_BOOLEN:
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

int main() {
    printf("--- MiniJS Type Test ---\n");

    // 1. Number Test
    JSValue s1 = createNUMBER(42.5);
    printf("Number Test: ");
    printValue(s1);

    // 2. String Test
    JSValue s2 = createSTRING("Hello C World");
    printf("Text Test: ");
    printValue(s2);

    // 3. Boolean Test
    JSValue s3 = createBOOLEN(1);
    JSValue s4 = createBOOLEN(0);
    printf("Boolean Test 1:");
    printValue(s3);
    printf("Boolean Test 0:");
    printValue(s4);

    // 4. Null and Undefined Test
    JSValue s5 = createNULL();
    JSValue s6 = createUNDEFINED();
    printf("NULL Test 1: ");
    printValue(s5);
    printf("UNDEFINED Tets 2: ");
    printValue(s6);

    return 0;
}