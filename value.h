#ifndef minijs_value_h
#define minijs_value_h

/**
 * @file value.h
 * @brief Defines the core data types and structures for the Aura language.
 *
 * This file contains the definition of `AuraValue`, which is a tagged union
 * representing all possible values in the language (primitives and objects),
 * along with their specific type enumerations and helper structures.
 */

#include "common.h"

/**
 * @brief Enumeration of all supported Aura data types.
 *
 * Distinguishes between primitive values (passed by value) and reference objects
 * (passed by reference/pointer).
 */
typedef enum {
    // --- Primitive (Value) Types ---
    AURA_UNDEFINED,
    AURA_NULL,
    AURA_BOOLEAN,
    AURA_NUMBER,
    AURA_STRING,
    AURA_SYMBOL,
    AURA_BIGINT,

    // --- AURA SPECIAL TYPES ---
    AURA_TENSOR,
    AURA_VEC3,

    // --- Object (Reference) Types ---
    AURA_OBJECT,
    AURA_ARRAY,
    AURA_DATE,
    AURA_MAP,
    AURA_SET,
    AURA_WEAKMAP,
    AURA_WEAKSET,
    AURA_FUNCTION
} AuraType;

/**
 * @brief Represents a 3-dimensional vector.
 * Used for mathematical and graphical operations.
 */
typedef struct {
    float x, y, z;
} AuraVec3;

/**
 * @brief Represents a multi-dimensional tensor structure.
 *
 * Designed for high-performance numerical computations.
 * Uses a Flexible Array Member for `data` to ensure spatial locality and reduce cache misses.
 */
typedef struct {
    size_t rows;
    size_t cols;
    float data[];
} AuraTensor;


/**
 * @brief Represents a dynamic Aura value.
 * 
 * Uses a "Tagged Union" pattern where the `type` field indicates which member
 * of the `union` is currently valid and active.
 */
typedef struct {
    AuraType type;

    union
    {
        double number;
        int boolean;
        char* string;
        char* symbol;
        long long bigint;
        AuraVec3 vec3;
        AuraTensor* tensor;
        void* object;
        void* function;
    } as;
} AuraValue;

// --- Value Creation & Management ---
AuraValue createUNDEFINED();
AuraValue createNULL();
AuraValue createBOOLEAN(int val);
AuraValue createNUMBER(double val);
AuraValue createSTRING(char* val);
AuraValue createBIGINT(long long val);
AuraValue createVEC3(float x, float y, float z);
AuraValue createTENSOR(int rows, int cols);

void printValue(AuraValue v);
void freeValue(AuraValue v);

#endif