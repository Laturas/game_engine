#define DEBUG
#include "afterhours.c"

/* The output might get annoying. */
// #define TESTCASE_STRINGS

void test_strings() {
	Arena strings_arena = {0};

	String current_directory = (String) {
		.str = "",
		.length = 0
	};

	StringArray array = fs_get_files_in_dir(&strings_arena, current_directory);

	ASSERT(array.len > 0);
	ASSERT(array.strings != NULL);

	for (int i = 0; i < array.len; i++) {
		printf("%.*s\n", array.strings[i].length, array.strings[i].str);
	}

	String models_directory = (String) {
		.str = "assets/models",
		.length = sizeof("assets/models")
	};

	printf("\nrelative directory: %.*s\n", models_directory.length, models_directory.str);

	array = fs_get_files_in_dir(&strings_arena, models_directory);
	
	ASSERT(array.len > 0);
	ASSERT(array.strings != NULL);

	for (int i = 0; i < array.len; i++) {
		printf("%.*s\n", array.strings[i].length, array.strings[i].str);
	}

	arena_free(&strings_arena);
}

void test_hashmap() {
	Arena hash_arena = {0};
	HashMap map = {0};

	StringArray array = fs_get_files_in_dir(&hash_arena, (String) {.str = "", .length = 0});

	for (int i = 0; i < array.len; i++) {
		String current = array.strings[i];
		int* value = arena_alloc(&hash_arena, sizeof(*value));
		ASSERT(value != NULL);

		u64 hash_position = hash_push(&map, &hash_arena, current.str, current.length, value);

		*value = (int)hash_position;
	}

	printf("%s", "Inserted. Retrieving values..\n\n");

	for (int i = 0; i < array.len; i++) {
		String current = array.strings[i];
		void* value = hash_get(map, current.str, current.length);
		ASSERT(value != NULL);

		int retrieved_value = *((int*)value);

		printf("%.*s : %d\n", current.length, current.str, retrieved_value);
	}

	printf("COLLISIONS: %d\n", collisions);

	arena_free(&hash_arena);
}

int main() {
	#ifdef TESTCASE_STRINGS
		printf("Testing strings\n");
		test_strings();
		printf("\nString test done\n");
	#endif

	printf("Testing hash maps\n");
	test_hashmap();
	printf("Hash map test passed\n");
}