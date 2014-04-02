#ifndef _PERMALLOC_H
#define _PERMALLOC_H

#include <cstddef>
#include <new>

namespace permalloc {

	// init functions
	void init (std::size_t s);
	void placement_init (void* base, std::size_t s);

	// tell permalloc data from foreign data
	bool is_from (void* p);
	template<typename T> inline
	void safe_delete (T* p)
	{
		if (is_from (p))
			;
		else
			delete p;
	}
	ptrdiff_t get_offset (void* p);

	// allocators
	void* raw (std::size_t nbytes) throw();
	template<typename T> inline
	T* obj () throw()
	{
		void* raw_obj = raw (sizeof(T));
		return new(raw_obj) T;
	}
	// definitely should have a variadic template here
	template<typename T, typename U> inline
	T* obj (U arg1) throw()
	{
		void* raw_obj = raw (sizeof(T));
		return new(raw_obj) T(arg1);
	}
	// 4-ary version to create sc_threads and sc_methods
	template<typename T, typename A, typename B,
		 typename C, typename D, typename E> inline
	T* obj (A a, B b, C c, D d, E e) throw()
	{
		void* raw_obj = raw (sizeof(T));
		return new(raw_obj) T(a,b,c,d,e);
	}
}

#endif

