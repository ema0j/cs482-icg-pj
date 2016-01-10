#ifndef _THREAD_UTILS_H_
#define _THREAD_UTILS_H_

#include <assert.h>

#define THREAD_FUNCTION(CLASSNAME, FUNCTION_NAME) static void* FUNCTION_NAME(void *ptr) \
	{ \
		assert(ptr); \
		if(ptr == NULL) return NULL; \
		CLASSNAME *clsptr = static_cast<CLASSNAME*>(ptr); \
		clsptr->FUNCTION_NAME(); \
		return NULL; \
	}


#define START_THREADS(N, THREADS, FUNCTION_NAME) THREADS.resize(N); \
	for (uint64_t idx = 0; idx < THREADS.size(); idx++) { pthread_create(&THREADS[idx], NULL, FUNCTION_NAME, this); } 

#define JOIN_THREADS(THREADS) for (uint64_t idx = 0; idx < THREADS.size(); idx++) { pthread_join(THREADS[idx], NULL); }

#endif // _THREAD_UTILS_H_