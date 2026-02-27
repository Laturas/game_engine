#pragma once

/**
* This is Katelyn's mini standard library of various utilities that she commonly uses across projects
* Feel free to reuse with credit
*/

#ifndef COMMON
#define COMMON

/**
* These are the types used in this project, but not the only types used in the project
*
* If something in this project is typed as "int", that means that the exact size is unimportant.
* Similarly, "float" is used for floating point numbers in which the exact precision is not as important.
*
* short, long, long long, and double are never used. For data types in which the size or signedness is important,
* they will use one of the following typedefs.
*
* "char" is used for actual string characters and they are assumed to be UTF-8. For byte
* manipulation, u8 is used instead to represent a single byte.
*
* If your compiler does not support static_assert(x), feel free to delete them
*/

typedef char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef float f32;
typedef double f64;

typedef u8 b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;
#ifndef bool
	#define bool int
#endif

/**
* rawptr is some type large enough to hold a pointer.
* This is for doing raw arithmetic on them.
*/
typedef u64 rawptr;
#ifndef true
	/**
	* Unsized truthy value.
	* This value is _never_ compared directly against in the form of if (x == true)
	*/
	#define true (bool)1
#endif

#ifndef false
	/**
	* Unsized falsy value.
	* This value is _never_ compared directly against in the form of if (x == false)
	*/
	#define false (bool)0
#endif

#ifndef NULL
	#define NULL (void*)0
#endif

#include <assert.h>

/**
* NULL in this project is assumed to be zero.
*
* The zero value specifically is often used for special cases throughout this project,
* as zero initialization is easy and prevents mistakes.
*/
#ifdef static_assert
	static_assert(sizeof(u8) == 1 , "Datatypes are the wrong size from what is expected");
	static_assert(sizeof(u16) == 2, "Datatypes are the wrong size from what is expected");
	static_assert(sizeof(u32) == 4, "Datatypes are the wrong size from what is expected");
	static_assert(sizeof(u64) == 8, "Datatypes are the wrong size from what is expected");

	static_assert(sizeof(i16) == 2, "Datatypes are the wrong size from what is expected");
	static_assert(sizeof(i32) == 4, "Datatypes are the wrong size from what is expected");
	static_assert(sizeof(i64) == 8, "Datatypes are the wrong size from what is expected");

	static_assert(sizeof(rawptr) == sizeof(void*), "Datatypes are the wrong size from what is expected");
#endif

/**
* This should never make it into the final product. This is only for testing purposes.
*
* Locally persistent variables are ones in local scopes that exist for the lifetime of the program.
*/
#define local_persistent static

/**
* For global variables. Please don't use these if you can help it.
* Prefer using global to denote global variables
*/
#define global static

#ifndef DISABLE_ASSSERTIONS
	#include <stdlib.h>
	#include <stdio.h>

	/**
	* Defined to exit the program. If optional_function is NULL it doesn't print it.
	*/
	void assertion_failure(int line, char* file, char* optional_function, char* expr) {
		fprintf(stderr, "Assertion failure at line: %d, file: %s", line, file);
		if (optional_function != NULL) {
			fprintf(stderr, ", function: %s\n", optional_function);
		} else {
			fprintf(stderr, "\n");
		}

		fprintf(stderr, "\t%s\n", expr);
		exit(-1);
	}

	#ifdef __func__
		#define ASSERTION_FUNCTION __func__
	#else
		#define ASSERTION_FUNCTION NULL
	#endif

	/**
	* A provably correct assertion used for delineating things like preconditions
	*/
	#define ASSERT(x) if(!(x)) assertion_failure(__LINE__, __FILE__, ASSERTION_FUNCTION, #x)
	/**
	* Indicates a code path should never happen (though the code should still function if it does)
	*
	* Used for conditions that are not known for certain to never occur
	*/
	#define NEVER(x) ((x)) ? assertion_failure(__LINE__, __FILE__, ASSERTION_FUNCTION, #x), (x) : (x)
	/**
	* Indicates a code path should always happen (though the code should still function if it doesn't)
	*
	* Used for conditions that are not known for certain to never occur
	*/
	#define ALWAYS(x) (!(x)) ? assertion_failure(__LINE__, __FILE__, ASSERTION_FUNCTION, #x), (x) : (x)

#endif

bool is_power_of_two(int x) {
	return (x & (x - 1)) == 0;
}

typedef struct Arena {
	void* bytes;
	i64 total_reserved_bytes;
	i64 first_unallocated_byte;
	i64 total_committed_bytes;
} Arena;

#define DEFAULT_MEMORY_ALIGNMENT (2*(sizeof(void*)))
#define PAGE_SIZE 4096
#define DEFAULT_MEMORY_RESERVATION (PAGE_SIZE*1024)

/**
* Aligns the given value to alignment
* e.g. alignment to 8 bytes would map 5 -> 8, 9 -> 16, etc.
*/
u64 align_forward(u64 to_align, int alignment) {
	ASSERT(is_power_of_two(alignment));
	ASSERT(alignment > 0);

	int remainder = to_align & (alignment - 1);

	if (remainder) {
		to_align += (alignment - remainder);
	}
	return to_align;
}

/**
* Reserves reservation_size address space at the returned pointer.
* Returns NULL on failure.
*/
void* platform_dependent_mem_reserve(u64 reservation_size);

/**
* Commits the memory to a reserved chunk of address space.
* Returns NULL on failure
*/
void* platform_dependent_mem_commit(void* commit_at, u64 commit_size);

/**
* Decommits the memory, allowing it to return to the operating system
*/
void platform_dependent_mem_decommit(void* addr, u64 decommit_size);

#ifdef linux
	#include <sys/mman.h>

	void* platform_dependent_mem_reserve(u64 reservation_size) {
		void* reservation = mmap(NULL, reservation_size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		return reservation;
	}

	void* platform_dependent_mem_commit(void* commit_at, u64 commit_size) {
		if (mprotect(commit_at, commit_size, PROT_READ|PROT_WRITE)) {
			return NULL;
		} else {
			return commit_at;
		}
	}

	void platform_dependent_mem_decommit(void* addr, u64 decommit_size) {
		/* Pages are made inaccessible again */
		mprotect(addr, decommit_size, PROT_NONE);

		/* Pages can be reclaimed */
		madvise(addr, decommit_size, MADV_DONTNEED);
	}
#endif

#ifdef _WIN32
	#include <memoryapi.h>
	void* platform_dependent_mem_reserve(u64 reservation_size) {
		return VirtualAlloc(NULL, reservation_size, MEM_RESERVE, PAGE_READWRITE);
	}

	void* platform_dependent_mem_commit(void* commit_at, u64 commit_size) {
		return VirtualAlloc(commit_at, commit_size, MEM_COMMIT, PAGE_READWRITE);
	}

	void platform_dependent_mem_decommit(void* addr, u64 decommit_size) {
		(void)decommit_size; /* This is only needed on linux */

		VirtualFree(addr, 0, MEM_RELEASE);
	}
#endif

void arena_init(Arena* arena, u64 reservation_size) {
	arena->bytes = platform_dependent_mem_reserve(reservation_size);
	arena->total_reserved_bytes = reservation_size;
	arena->first_unallocated_byte = 0;
}

u64 round_to_page_size(u64 input) {
	return (input + (PAGE_SIZE - (input % PAGE_SIZE)));
}

void* arena_alloc(Arena* arena, u64 byte_count) {
	if (NEVER(arena == NULL)) return NULL;

	if (arena->bytes == NULL) {
		arena_init(arena, DEFAULT_MEMORY_RESERVATION);
	}

	i64 push_to = align_forward(byte_count + arena->first_unallocated_byte, DEFAULT_MEMORY_ALIGNMENT);

	u64 total_committed_bytes = round_to_page_size((u64)push_to);

	if (push_to >= arena->total_committed_bytes) {
		if (platform_dependent_mem_commit(arena->bytes, total_committed_bytes) == NULL) {
			return NULL;
		}
	}
	
	/* Pointer arithmetic on void pointers are technically undefined behavior */
	void* ret = (void*)((rawptr)arena->bytes + arena->first_unallocated_byte);
	arena->first_unallocated_byte = push_to;
	arena->total_committed_bytes = total_committed_bytes;

	return ret;
}

/**
* Returns the current byte position of the arena for later use in an arena_restore
*/
u64 arena_save(Arena* arena) {
	return arena->first_unallocated_byte;
}

/**
* Shrinks the stack size to an old position
* Note that this does not decommit the pages.
*/
void arena_restore(Arena* arena, u64 position) {
	arena->first_unallocated_byte = position;
}

/**
* Completely frees the arena, decommitting the pages and unreserving the address space.
*/
void arena_free(Arena* arena) {
	if (arena->bytes == NULL) return;
	
	platform_dependent_mem_decommit(arena->bytes, arena->total_reserved_bytes);
}

typedef struct string {
	char* str;
	int length;
} String;

String string_null_to_length_terminated(char* null_terminated_string) {
	int i = 0;
	while (null_terminated_string[i] != '\0') i++;

	return (String){.str = null_terminated_string, .length = i};
}

/**
* Returns whether the two input strings are equivalent
*/
bool string_eq(String str_1, String str_2) {
	if (str_1.length != str_2.length) {
		return false;
	}

	for (int i = 0; i < str_1.length; i++) {
		if (str_1.str[i] != str_2.str[i]) {
			return false;
		}
	}

	return true;
}

/**
 * Returns a new string equal to the concatenation of two smaller strings.
 */
String string_concatenate(Arena* string_arena, String str_1, String str_2) {
	int combined_len = str_1.length + str_2.length;
	char* buffer = arena_alloc(string_arena, sizeof(char) * (combined_len + 1));
	if (buffer == NULL) { return (String) { .str = NULL, .length = 0 }; }

	for (int i = 0; i < str_1.length; i++) {
		buffer[i] = str_1.str[i];
	}

	for (int i = str_1.length; i < combined_len; i++) {
		buffer[i] = str_2.str[i];
	}

	buffer[combined_len] = '\0';

	return (String) {
		.length = combined_len,
		.str = buffer
	};
}

#ifdef linux
	#define FILE_SEPARATOR ('/')
#endif
#ifdef WIN32
	#define FILE_SEPARATOR ('\\')
#endif
#ifndef FILE_SEPARATOR
	#error "File separator not defined for this platform"
#endif

String string_copy(Arena* string_arena, String copy_from) {
	char* str = arena_alloc(string_arena, copy_from.length + 1);

	for (int i = 0; i < copy_from.length; i++) {
		str[i] = copy_from.str[i];
	}
	str[copy_from.length] = '\0';

	return (String) {
		.length = copy_from.length,
		.str = str
	};
}

/**
 * Concatenates two file strings in a safe, cross-platform manner.
 * Ensures that they're null terminated.
 * 
 * Windows:
 *    file\path + file.txt = file\path\file.txt
 * Linux:
 *    file/path + file.txt = file/path/file.txt
 * etc.
 * 
 * This is fairly robust. It handles the case where the second string starts with, or the first string ends with, the file separator.
 * Also handles the case where one or both strings are empty.
 */
String string_concatenate_files(Arena* string_arena, String str_1, String str_2) {
	int combined_len = str_1.length + str_2.length;
	int extra_concat = (
		(str_1.length != 0 && str_2.length != 0) && (
			(str_1.str[str_1.length - 1] == FILE_SEPARATOR) ||
			(str_2.str[0] == FILE_SEPARATOR)
		)
	);

	char* buffer = arena_alloc(string_arena, sizeof(char) * (combined_len + extra_concat + 1));
	if (buffer == NULL) { return (String) { .str = NULL, .length = 0 }; }

	for (int i = 0; i < str_1.length; i++) {
		buffer[i] = str_1.str[i];
	}

	for (int i = str_1.length; i < combined_len; i++) {
		buffer[i] = str_2.str[i];
	}

	buffer[combined_len + extra_concat] = '\0';

	return (String) {
		.length = combined_len + extra_concat,
		.str = buffer
	};
}

typedef struct StringArray {
	String* strings;
	int len;
} StringArray;

StringArray platform_dependent_get_all_files_in_directory(Arena* strings_arena, String directory);

#ifdef linux
#include <dirent.h> 
#include <sys/stat.h>

bool is_regular_file_internal(struct dirent* file) {
	if (file->d_type == DT_REG) {
		return true;
	}
	else if (file->d_type == DT_UNKNOWN) {
		struct stat st;
		if (stat(file->d_name, &st) == 0) {
			if (S_ISREG(st.st_mode)) {
				return true;
			}
		}
	}
	return false;
}

StringArray platform_dependent_get_all_files_in_directory(Arena* strings_arena, String directory) {
	StringArray array = { .strings = NULL, .len = 0 };

	/* We can't assume that the string is null terminated. It could be a string slice. */
	int prev = arena_save(strings_arena);
		char* directory_name = arena_alloc(strings_arena, directory.length + 1);

		for (int i = 0; i < directory.length; i++) {
			directory_name[i] = directory.str[i];
		}
		directory_name[directory.length] = '\0';

		DIR* directory_stream = opendir(directory_name);
		DIR* directory_stream_2 = opendir(directory_name);
	arena_restore(strings_arena, prev);

	if (directory_stream != NULL && directory_stream_2 != NULL) {
		/* There might be a better way but I wanted to do it all in one allocation so I just loop over it twice. */
		int count = 0;
		for (struct dirent* file = readdir(directory_stream); file != NULL; file = readdir(directory_stream)) {
			if (is_regular_file_internal(file)) {
				String str = string_null_to_length_terminated(file->d_name);
	
				if (!string_eq(str, (String) {.length = 1, .str = "."}) &&
					!string_eq(str, (String) {.length = 2, .str = ".."})
				) {
					count++;
				}
			}
		}
		closedir(directory_stream);

		array.strings = arena_alloc(strings_arena, sizeof(*array.strings) * count);
		array.len = count;

		int i = 0;

		for (struct dirent* file = readdir(directory_stream_2); file != NULL; file = readdir(directory_stream_2)) {
			if (is_regular_file_internal(file)) {
				String str = string_null_to_length_terminated(file->d_name);
	
				if (!string_eq(str, (String) {.length = 1, .str = "."}) &&
					!string_eq(str, (String) {.length = 2, .str = ".."})
				) {
					array.strings[i] = str;
					i++;
				}
			}
		}		
	}

	return array;
}
#endif

StringArray fs_get_files_in_dir(Arena* strings_arena, String directory) {
	return platform_dependent_get_all_files_in_directory(strings_arena, directory);
}

/**
* Indicates that an expression is unused. Gets compiled out.
* This is so we can keep them around without them affecting overhead, and keeping documentation.
*/
#ifdef UNUSED
	#undef UNUSED
#endif

#endif