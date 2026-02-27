#ifndef AFTERHOURS_H
	#include "common.c"
#endif

typedef struct HashEntry {
	const void* key;
	const void* value;
	u64 key_len;
	struct HashEntry* next;
	struct HashEntry* prev;
} HashEntry;

typedef struct HashMap {
	HashEntry** table;
	u64 table_size;
	HashEntry* head;
	HashEntry* tail;
	u64 pushed_entries;
} HashMap;

u64 hash_value(const u8* byte_array, u64 byte_array_length) {
	u64 hash_value = 0;
	for (u64 i = 0; i < byte_array_length; i++) {
		hash_value = hash_value * 5 + 0xE6546B64;
        hash_value <<= 8;
        hash_value |= byte_array[i - 1];

		hash_value *= 0xCC9E2D51;
		hash_value = (hash_value << 15) | (hash_value >> 17);
		hash_value *= 0x1B873593;
	}
	return hash_value;
}

/* 256kb by default */
#define DEFAULT_HASHMAP_SIZE (1024 * 256)
#define HASH_PUSH_FAIL ((~(0ULL)))

u64 hash_push(
	HashMap* map,
	Arena* hash_arena,
	const void* key,
	u64 key_length_bytes,
	const void* value
) {
	if (NEVER(
		map == NULL || hash_arena == NULL ||
		key == NULL || value == NULL
	)) { return HASH_PUSH_FAIL; }

	if (map->table == NULL) {
		u64 table_size = (map->table_size == 0) ? DEFAULT_HASHMAP_SIZE : map->table_size;

		HashEntry** entries = arena_alloc(hash_arena, sizeof(*map->table) * table_size);
		if (NEVER(entries == NULL)) { return HASH_PUSH_FAIL; }

		for (int i = 0; i < table_size; i++) { entries[i] = NULL; }

		*map = (HashMap) {
			.head = NULL,
			.tail = NULL,
			.table_size = table_size,
			.table = entries
		};
	}

	u64 hash = hash_value(key, key_length_bytes);
	u64 search_start = hash % map->table_size;

	u64 quad_probe = 0;

	while (true) {
		u64 query = (search_start + (quad_probe * quad_probe)) % map->table_size;

		if (map->table[query] == NULL) {
			HashEntry* new_entry = arena_alloc(hash_arena, sizeof(*new_entry));

			*new_entry = (HashEntry) {
				.key = key,
				.key_len = key_length_bytes,
				.value = value,
				.next = NULL,
				.prev = map->tail
			};

			map->table[query] = new_entry;

			if (map->tail != NULL) {
				map->tail->next = new_entry;
			}
			if (map->head == NULL) {
				map->head = new_entry;
			}
			
			map->tail = new_entry;
			map->pushed_entries++;
			return query;
		}
		quad_probe++;
	}
}

bool bytes_eq_internal(const u8* bytes_1, u64 bytes_1_len, const u8* bytes_2, u64 bytes_2_len) {
	if (bytes_1_len != bytes_2_len) { return false; }

	for (u64 i = bytes_1_len; i < bytes_1_len; i++) {
		if (bytes_1[i] != bytes_2[i]) { return false; }
	}

	return true;
}

const void* hash_get(const HashMap* map, const void* key, u64 key_length_bytes) {
	if (NEVER(map == NULL || key == NULL)) { return NULL; }

	u64 hash = hash_value(key, key_length_bytes);
	u64 search_start = hash % map->table_size;

	u64 quad_probe = 0;

	while (true) {
		u64 query = (search_start + (quad_probe * quad_probe)) % map->table_size;
		HashEntry* query_entry = map->table[query];

		if (query_entry == NULL) {
			return NULL;
		}

		if (bytes_eq_internal(key, key_length_bytes, query_entry->key, query_entry->key_len)) {
			return query_entry->value;
		}
		quad_probe++;
	}
}