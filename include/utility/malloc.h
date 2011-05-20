// EPOS Application-level Dynamic Memory Utility Declarations

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

#ifndef __malloc_h
#define __malloc_h

#include <application.h>

extern "C"
{
    inline void * malloc(size_t bytes) {
	return __SYS(Application)::heap()->alloc(bytes);
    }

    inline void * calloc(size_t n, unsigned int bytes) {
	return __SYS(Application)::heap()->calloc(n * bytes);
    }

    inline void free(void * ptr) {
	__SYS(Application)::heap()->free(ptr);
    }
}

inline void * operator new(size_t bytes) {
    return malloc(bytes);
}

inline void * operator new[](size_t bytes) {
    return malloc(bytes); 
}

inline void operator delete(void * object) {
    free(object);
}

inline void operator delete[](void * object) {
    free(object);
}

#endif
