#include "afterhours.c"

/* The output might get annoying. */
#define TESTCASE_STRINGS

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

int main() {
	#ifdef TESTCASE_STRINGS
		printf("Testing strings\n");
		test_strings();
		printf("\nString test done\n");
	#endif
}