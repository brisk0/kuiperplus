/* © 2017 Kaylee Blake. License to be advised
 *
 * Flexible arrays for C with a pleasant API
 * =========================================
 *
 * Declaration: Array(<element_type>,<array_name>);
 *
 * Append:      array_append(<array_name>, <element_name>)
 *
 * Index:       <array_name>[<index>]
 *
 * Iterate:     forval(<value_of_element>, <name_of_array>) {
 *                  dosomething(<value_of_element>);
 *              }
 *          OR
 *              forptr(<ptr_to_element>) {
 *                  dosomething(<ptr_to_element>);
 *              }
 */

#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Declarations syntax: Array(<arraytype>,<arrayname>);
#define Array(type, name) uint32_t name##_cap = 0; uint32_t name##_count = 0; type *name = NULL

// For use inside structures
#define ArrayElement(type, name) struct {uint32_t name##_cap; uint32_t name##_count; type *name; }

// XXX Candidate for deletion
#define Slice(type, name) struct {uint32_t name##_count; type *name; }

// Use to set / expand the base capacity of an array. Good for performance if
// you're about to add a whole lot of elements or know how big it's likely to
// get.
#define array_reserve(array, num_to_reserve) \
	((typeof(array))array_reserve_(&(array##_cap), \
	                             &(array##_count), \
	                             (uint8_t **)&(array), \
	                             sizeof(*(array)), \
	                             (num_to_reserve)))

__attribute__((always_inline))
inline void *array_reserve_(uint32_t *cap, uint32_t *count, uint8_t **data, uint32_t elem_size, uint32_t num_to_reserve) {
	if (*data == NULL) {
		if (num_to_reserve >= 2) {
			*cap = num_to_reserve;
		} else {
			*cap = 2;
		}
		*count = 0;
		*data = malloc(*cap * elem_size);
	} else if (*count + num_to_reserve > *cap) {
		uint32_t wanted = *count + num_to_reserve;
		while (wanted > *cap) {
			*cap = *cap + (*cap >> 1);
		}
		*data = realloc(*data, *cap * elem_size);
	}
	return *data + *count * elem_size;
}

#define array_alloc(array, num_to_alloc) \
	((typeof(array))array_alloc_(&(array##_cap), \
	                           &(array##_count), \
	                           (uint8_t **)&(array), \
	                           sizeof(*(array)), \
	                           (num_to_alloc)))

__attribute__((always_inline))
inline void *array_alloc_(uint32_t *cap, uint32_t *count, uint8_t **data, uint32_t elem_size, uint32_t num_to_alloc) {
	void *result = array_reserve_(cap, count, data, elem_size, num_to_alloc);
	*count += num_to_alloc;
	return result;
}

// Add element to array
#define array_append(array, elem) \
	(*(typeof(array))array_append_(&(array##_cap), \
	                             &(array##_count), \
	                             (uint8_t **)&(array), \
	                             sizeof(*(array))) = elem)

__attribute__((always_inline))
inline void *array_append_(uint32_t *cap, uint32_t *count, uint8_t **data, uint32_t elem_size) {
	if (*data == NULL) {
		*cap = 8;
		*count = 0;
		*data = malloc(*cap * elem_size);
	} else if (*count + 1 > *cap) {
		uint32_t wanted = *count + 1;
		while (wanted > *cap) {
			*cap = *cap + (*cap >> 1);
		}
		*data = realloc(*data, *cap * elem_size);
	}
	return *data + (*count)++ * elem_size;
}

// Concat any c array (ptr) onto an Array (array).
#define array_concat(array, ptr, count) \
	do { \
		typeof(*(ptr)) __value; \
		memset(&__value, 0, sizeof(__value)); \
		typeof(*(array)) __assert_assignable = __value; \
		(void) __assert_assignable; \
		if (count) { \
			memcpy(array_alloc(array, (count)), (ptr), (count) * sizeof(*(array))); \
		} \
	} while (false)

#define array_pad(array, count) \
	memset(array_alloc(array, (count)), 0, (count) * sizeof(*(array)))

#define array_trim(array) \
	array_trim_(\
	          &(array##_cap), \
	          &(array##_count), \
	          (uint8_t **)&(array), \
	          sizeof(*(array)))

__attribute__((always_inline))
inline void array_trim_(uint32_t *cap, uint32_t *count, uint8_t **data, uint32_t elem_size) {
	if (*count == *cap) {
		return;
	}
	*data = realloc(*data, *count * elem_size);
	*cap = *count;
}

#define forval(var, array) \
	for (uint32_t var##_index = 0; var##_index < array##_count; var##_index = -1u) \
	for (typeof(array[0]) var; \
	     var##_index < array##_count ? (var = array[var##_index], true) : false; \
	     var##_index++)

#define forptr(var, array) \
	for (uint32_t var##_index = 0; var##_index < array##_count; var##_index = -1u) \
	for (typeof(array[0]) *var; \
	     var##_index < array##_count ? (var = &array[var##_index], true) : false; \
	     var##_index++)

#endif
