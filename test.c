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

void test_ends_with() {
	String str_1 = {.str = "Hello.mp3", .length = sizeof("Hello.mp3") - 1};
	String str_2 = {.str = ".mp3", .length = sizeof(".mp3") - 1};

	ASSERT(string_ends_with(str_1, str_2));
	ASSERT(!string_ends_with(str_2, str_1));
	ASSERT(string_ends_with(str_2, str_2));
}

void test_raycasting() {
	Arena collision_arena = {0};

	TriangleCollider tri = {
		.entity_id = 0,
		.mask = MASK_ALL,
		.vert_1 = (Vector3) {.x = -50, .y = 0, .z = -50},
		.vert_2 = (Vector3) {.x = 50, .y = 0, .z = -50},
		.vert_3 = (Vector3) {.x = 50, .y = 0, .z = 50},
	};

	TriangleColliderArray arr = {
		.colliders = &tri,
		.length = 1
	};

	#ifdef DEFAULT_CELL_WIDTH
		#undef DEFAULT_CELL_WIDTH
	#endif

	SpacialHash hash = collision_spacial_hash_create(&collision_arena, arr);

	Vector3 hit = collision_ray_intersection_with_aabb(&hash, VECTOR3_ZERO, (Vector3) {1.0f, 1.0f, 1.0f}, 20.0f);

	ASSERT(Vector3Equals(hit, VECTOR3_ZERO)); /* This should return the zero vector, because the starting point (origin) is inside the hash */

	hit = collision_ray_intersection_with_aabb(&hash, (Vector3) {-100.0f, -100.0f, -100.0f}, (Vector3) {1.0f, 1.0f, 1.0f}, INFINITY);

	printf("hitpt 2 = (%f, %f, %f)\n", hit.x, hit.y, hit.z);
}

int main() {
	#ifdef TESTCASE_STRINGS
		printf("Testing strings\n");
		test_strings();
		printf("\nString test done\n");
	#endif

	#ifdef TESTCASE_HASHMAP
		printf("Testing hash maps\n");
		test_hashmap();
		printf("Hash map test passed\n");
	#endif

	printf("Testing string ends with\n");
	test_ends_with();
	printf("string ends with test passed\n");


	printf("Testing raycasting\n");
	test_raycasting();
	printf("Raycasting test passed\n");
}