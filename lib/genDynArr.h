#ifndef GENDYNARR_H
#define GENDYNARR_H

#include <stdlib.h>

#define DEFINE_DYNARR(T, Name) \
	typedef struct{ \
		T* data; \
		size_t size; \
		size_t len; \
	}Name; \
	\
	static inline void Name##_init(Name* arr){ \
		arr->data = NULL; \
		arr->size = 0; \
		arr->len = 0; \
	} \
	\
	static inline void Name##_push(Name* arr, T val){ \
		if(arr->size == arr->len){ \
			size_t newSize = (arr->len == 0) ? 2 : arr->size * 2; \
			T* newData = (T*)realloc(arr->data, newSize * sizeof(T)); \
			if(!newData) return; \
			arr->data = newData; \
			arr->size = newSize; \
		} \
		arr->data[arr->len++] = val; \
	} \
	\
	static inline void Name##_free(Name* arr){ \
		free(arr->data); \
		arr->data = NULL; \
		arr->size = 0; \
		arr->len = 0; \
	}

#endif