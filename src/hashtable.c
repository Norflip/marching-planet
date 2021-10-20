#include <string.h>
#include <stdio.h>
#include "hashtable.h"

struct HT_bucket 
{
    uint64_t key;
    void* value;
    bool used;
};

static inline uint64_t offset_hash (uint64_t x) 
{
    x = ((x >> 16) ^ x) * 0x45d9f3bUL;
    x = ((x >> 16) ^ x) * 0x45d9f3bUL;
    x = (x >> 16) ^ x;
    return x;
}

Hashtable* hashtable_new (size_t capacity)
{
    size_t size = sizeof(Hashtable) + capacity * sizeof(struct HT_bucket);
    void* mem = HASHTABLE_MALLOC(size);
    memset(mem, 0, size);
    
    Hashtable* table = mem;
    table->capacity = capacity;
    table->count = 0;
    table->buckets = (uintptr_t)mem + sizeof(Hashtable); 
    return table;
}

void hashtable_set(Hashtable* table, int64_t key, void* value)
{
    //rehash the key?
    size_t hash = offset_hash(key);

    for (size_t i = 0; i < table->capacity; i++)
    {
        size_t h = (i + hash + key) % table->capacity;

        if(!table->buckets[h].used)
        {
            table->buckets[h].key = key;
            table->buckets[h].value = value;
            table->buckets[h].used = true;
            table->count++;
            break;
        }
    }
}

void* hashtable_get(Hashtable* table, int64_t key)
{
    void* value = NULL;
    size_t hash = offset_hash(key);

    for (size_t i = 0; i < table->capacity; i++)
    {
        size_t h = (i + hash + key) % table->capacity;
        if(table->buckets[h].used && table->buckets[h].key == key)
        {
            value = table->buckets[h].value;
            break;
        }
    }

    return value;
}

bool hashtable_has(Hashtable* table, int64_t key)
{
    size_t hash = offset_hash(key);

    for (size_t i = 0; i < table->capacity; i++)
    {
        size_t h = (i + hash + key) % table->capacity;
        if(table->buckets[h].used && table->buckets[h].key == key)
        {
            printf("key %i already exists\n", key);
            return true;
        }
            
        if(table->buckets[h].value == NULL)
            return false;
    }
    return false;
}

void hashtable_remove (Hashtable* table, int64_t key)
{
    size_t hash = offset_hash(key);

    for (size_t i = 0; i < table->capacity; i++)
    {
        size_t h = (i + hash + key) % table->capacity;
        if(table->buckets[h].key == key)
        {
            table->buckets[h].key = 0;
            table->buckets[h].value = NULL;   
            table->buckets[h].used = false;
            break;
        }
    }
}

void hashtable_print(Hashtable* table)
{
    printf("--- TABLE ---\n");
    printf("Items: %i\n", table->count);
    
    for (size_t i = 0; i < table->capacity; i++)
    {
        printf("[%i] K(%i) %s\n", (int)i, (int)table->buckets[i].key, (char*)table->buckets[i].value);
    }
}