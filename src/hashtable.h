#ifndef __HASHTABLE__
#define __HASHTABLE__
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef HASHTABLE_MALLOC
    #define HASHTABLE_MALLOC malloc
#endif

// https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
#define UI64_2D(x,y) hash_cantorpairing(x,y)
static int64_t hash_cantorpairing (int32_t a, int32_t b)
{
    uint64_t A = (uint64_t)(a >= 0 ? 2 * (int64_t)a : -2 * (int64_t)a - 1);
    uint64_t B = (uint64_t)(b >= 0 ? 2 * (int64_t)b : -2 * (int64_t)b - 1);
    int64_t C = (int64_t)((A >= B ? A * A + A + B : A + B * B) / 2);
    return ((a < 0 && b < 0) || (a >= 0 && b >= 0)) ? C : -C - 1;
}

#define UI64_str(str) hash_str(str)
static uint64_t hash_str(unsigned char *str)
{
    uint64_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


typedef struct Hashtable
{
    struct HT_bucket* buckets;
    size_t capacity;
    size_t count;
} Hashtable;
 
Hashtable* hashtable_new (size_t capacity);
void hashtable_set(Hashtable* table, int64_t key, void* value);
void* hashtable_get(Hashtable* table, int64_t key);
bool hashtable_has(Hashtable* table, int64_t key);
void hashtable_remove (Hashtable* table, int64_t key);

void hashtable_dispose (Hashtable* table);

void hashtable_print(Hashtable* table);


#endif //__HASHTABLE__