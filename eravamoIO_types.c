#include "eravamoIO_types.h"

void cleanStringArray(StringArray *a){
	for(size_t i = 0; i < a->len; i++){
		free(a->data[i]);
	}
	StringArray_free(a);
}
