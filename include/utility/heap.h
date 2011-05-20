// EPOS Heap Utility Declarations

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

#ifndef __heap_h
#define __heap_h

#include <utility/debug.h>
#include <utility/string.h>
#include <utility/list.h>
#include <utility/spin.h>

__BEGIN_SYS

// Heap Common Package (actually the non-atomic heap)
class Heap_Common: private Grouping_List<char>
{
public:
    using Grouping_List<char>::empty;
    using Grouping_List<char>::size;

    Heap_Common() {
	db<Init, Heap>(TRC) << "Heap() => " << this << "\n";
    }

    Heap_Common(void * addr, unsigned int bytes) {
	db<Init, Heap>(TRC) << "Heap(addr=" << addr << ",bytes=" << bytes 
			    << ") => " << this << "\n";  
    }

    void * alloc(unsigned int bytes); 

    void * calloc(unsigned int bytes); 

    void free(void * ptr); 

    void free(void * ptr, unsigned int bytes);

private:
    void out_of_memory();
};


// Wrapper for non-atomic heap  
template <bool atomic>
class Heap_Wrapper: public Heap_Common {};


// Wrapper for atomic heap
template<>
class Heap_Wrapper<true>: public Heap_Common
{
public:
    Heap_Wrapper() {}

    Heap_Wrapper(void * addr, unsigned int bytes): Heap_Common(addr, bytes) {
	free(addr, bytes); 
    }

    void * alloc(unsigned int bytes) {
	_lock.acquire();
	void * tmp = Heap_Common::alloc(bytes);
	_lock.release();
	return tmp;
    }

    void * calloc(unsigned int bytes) {
	_lock.acquire();
	void * tmp = Heap_Common::calloc(bytes);
	_lock.release();
	return tmp;
    }

    void free(void * ptr) {
	_lock.acquire();
	Heap_Common::free(ptr);
	_lock.release();
    }

    void free(void * ptr, unsigned int bytes) {
	_lock.acquire();
	Heap_Common::free(ptr, bytes);
	_lock.release();
    }

private:
    Spin _lock;
};


// Heap
class Heap: public Heap_Wrapper<Traits<Thread>::smp> {};

__END_SYS

#endif
