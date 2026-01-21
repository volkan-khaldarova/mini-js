#include "value.h"

// --- AUXILIARY CREATION FUNCTIONS ---

JSValue createUNDEFINED() {
    JSValue v;
    v.type = JS_UNDEFINED;
    return v;
}

JSValue createNULL() {
    JSValue v;
    v.type = JS_NULL;
    return v;
}

JSValue createBOOLEAN(int val) {
    JSValue v;
    v.type = JS_BOOLEAN;
   v.as.boolean = val;
    return v;
}

JSValue createNUMBER(double val) {
    JSValue v;
    v.type = JS_NUMBER;
    v.as.number = val;
    return v;
}

JSValue createSTRING(char* val) {
   JSValue v;
    v.type = JS_STRING;
    v.as.string = strdup(val);
    return v;
}

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

void freeValue(JSValue v) {
    if (v.type == JS_STRING) {
        free(v.as.string);
    }
}
