#ifndef AFTERHOURS_H
    #include "common.c"
#endif

typedef struct HashEntry {
    void* key;
    void* value;
    void* next;
} HashEntry;

typedef struct HashMap {
    HashEntry* bytes;
    u64 byte_array_size;
    void* head;
    void* tail;
} HashMap;

u64 hash_value(u8* byte_array, u64 byte_array_length) {
    
}

void hash_push(HashMap* map, void* key, void* value) {

}