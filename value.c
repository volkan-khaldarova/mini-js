/**
 * @file value.c
 * @brief Implementation of Aura value creation and management functions.
 *
 * Handles the allocation, initialization, printing, and memory deallocation
 * for various Aura data types, including primitives and complex structures like Tensors.
 */

#include "value.h"
#include <stdint.h>

// --- AUXILIARY CREATION FUNCTIONS ---

/**
 * Creates a AuraValue representing 'undefined'.
 *
 * @return A AuraValue with type AURA_UNDEFINED.
 * @complexity O(1)
 */
AuraValue createUNDEFINED() {
    AuraValue v;
    v.type = AURA_UNDEFINED;
    return v;
}

/**
 * Creates a AuraValue representing 'null'.
 *
 * @return A AuraValue with type AURA_NULL.
 * @complexity O(1)
 */
AuraValue createNULL() {
    AuraValue v;
    v.type = AURA_NULL;
    return v;
}

/**
 * Creates a AuraValue representing a boolean.
 *
 * @param val 1 for true, 0 for false.
 * @return A AuraValue with type AURA_BOOLEAN.
 * @complexity O(1)
 */
AuraValue createBOOLEAN(int val) {
    AuraValue v;
    v.type = AURA_BOOLEAN;
   v.as.boolean = val;
    return v;
}

/**
 * Creates a AuraValue representing a number.
 *
 * @param val The double precision floating point value.
 * @return A AuraValue with type AURA_NUMBER.
 * @complexity O(1)
 */
AuraValue createNUMBER(double val) {
    AuraValue v;
    v.type = AURA_NUMBER;
    v.as.number = val;
    return v;
}

/**
 * Creates a AuraValue representing a string.
 *
 * Duplicates the input string using `strdup` and performs robust error checking.
 *
 * @param val The C-string to wrap.
 * @return A AuraValue with type AURA_STRING.
 * @complexity O(N) where N is the length of the string.
 */
AuraValue createSTRING(char* val) {
    AuraValue v;
    if (val == NULL) {
        return createNULL();
    }

    v.type = AURA_STRING;
    v.as.string = strdup(val);
    
    if (v.as.string == NULL) {
        fprintf(stderr, "[Fatal Error] Out of memory in createSTRING.\n");
        return createNULL();
    }

    return v;
}

/**
 * Creates a AuraValue representing a BigInt.
 *
 * @param val The long long integer value.
 * @return A AuraValue with type AURA_BIGINT.
 * @complexity O(1)
 */
AuraValue createBIGINT(long long val) {
    AuraValue v;
    v.type = AURA_BIGINT;
    v.as.bigint = val;
    return v;
}

/**
 * Creates a AuraValue representing a 3D Vector.
 *
 * @param x X component.
 * @param y Y component.
 * @param z Z component.
 * @return A AuraValue with type AURA_VEC3.
 * @complexity O(1)
 */
AuraValue createVEC3(float x, float y, float z) {
    AuraValue v;
    v.type = AURA_VEC3;
    v.as.vec3.x = x;
    v.as.vec3.y = y;
    v.as.vec3.z = z;
    return v;
}

/**
 * Creates a AuraValue representing a Tensor.
 *
 * Allocates a contiguous memory block for both the tensor metadata and its data payload.
 * Includes rigorous security checks against integer overflows during size calculation.
 *
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @return A AuraValue with type AURA_TENSOR, or AURA_NULL if allocation fails or dimensions are invalid.
 * @complexity O(rows * cols) due to `calloc` zero-initialization.
 */
AuraValue createTENSOR(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "[Security] Invalid tensor dimensions.\n");
        return createNULL();
    }

    if (cols > 0 && (size_t)rows > SIZE_MAX / (size_t)cols) {
         fprintf(stderr, "[Security] Tensor size overflow detected.\n");
         return createNULL();
    }
    size_t total_elements = (size_t)rows * (size_t)cols;

    if (total_elements > SIZE_MAX / sizeof(float)) {
        fprintf(stderr, "[Security] Tensor allocation size overflow (multiplication).\n");
        return createNULL();
    }

    if (SIZE_MAX - sizeof(AuraTensor) < total_elements * sizeof(float)) {
        fprintf(stderr, "[Security] Tensor allocation size overflow (addition).\n");
        return createNULL();
    }

    AuraValue v;
    v.type = AURA_TENSOR;
    
    v.as.tensor = (AuraTensor*)calloc(1, sizeof(AuraTensor) + (sizeof(float) * total_elements));

    if (v.as.tensor == NULL) {
        fprintf(stderr, "[Fatal Error] Out of memory in createTENSOR.\n");
        return createNULL();
    }
    
    v.as.tensor->rows = (size_t)rows;
    v.as.tensor->cols = (size_t)cols;
    return v;
}

/**
 * Prints the value of a AuraValue to the standard output.
 *
 * Handles formatting for different types (e.g., 'n' suffix for BigInt, vector components).
 *
 * @param v The value to print.
 */
void printValue(AuraValue v) {
    switch (v.type)
    {
    case AURA_UNDEFINED:
        printf("undefined");
        break;
    case AURA_NULL:
        printf("null");
        break;
    case AURA_BOOLEAN:
        printf(v.as.boolean ? "true" : "false");
        break;
    case AURA_NUMBER:
        printf("%g", v.as.number);
        break;
    case AURA_STRING:
        printf("'%s'", v.as.string);
        break;
    case AURA_BIGINT:
        printf("%lldn", v.as.bigint);
        break;
    case AURA_VEC3:
        printf("Vec3(%g, %g, %g)", v.as.vec3.x, v.as.vec3.y, v.as.vec3.z);
        break;
    case AURA_TENSOR:
        printf("Tensor[%zux%zu]", v.as.tensor->rows, v.as.tensor->cols);
        break;
    case AURA_OBJECT:
        printf("[Object]");
        break;
    case AURA_ARRAY:
        printf("[Array]");
        break;
    case AURA_FUNCTION:
        printf("[Function]");
        break;
    default:
        printf("Unknown Type!");
        break;
    }
    printf("\n");
}

/**
 * Frees the memory allocated for a AuraValue.
 *
 * Handles manual garbage collection for dynamic types like String and Tensor.
 * Safe to call on primitive types (no-op).
 *
 * @param v The value to free.
 */
void freeValue(AuraValue v) {
    if (v.type == AURA_STRING) {
        free(v.as.string);
    }
    if (v.type == AURA_TENSOR) {
        free(v.as.tensor);
    }
}