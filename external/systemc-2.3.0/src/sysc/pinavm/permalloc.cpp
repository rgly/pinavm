#include <sysc/pinavm/permalloc.h>
#include <sysc/kernel/sc_module.h>
#include <sysc/kernel/sc_process_table.h>
#include <stdio.h>
#include <stdlib.h>

using namespace permalloc;

char* stack_base = 0;
// should be atomic if permalloc is to be reentrant
char* stack_top = 0;
size_t stack_size = 0;

// glibc optimizes it to mmap if size is big enough
void permalloc::init (size_t s)
{
	void* base = malloc (s);
	placement_init (base, s);
}

void permalloc::placement_init (void* base, size_t s)
{
	stack_base = reinterpret_cast<char*> (base);
	stack_top = stack_base;
	stack_size = s;
}

void* permalloc::raw (std::size_t nbytes) throw()
{
	// only use the stack if it's initialized
	if (stack_base) {
		char* ret_value = stack_top;
		stack_top += nbytes;
		if ((stack_top - stack_base) < 0
		 || (size_t) (stack_top - stack_base) > stack_size) {
			fprintf (stderr, "permalloc: out of memory\n");
			fprintf (stderr, "stack_base: %p stack_top: %p stack_size: %zx",
				 stack_base, stack_top, stack_size);
			abort();
			return 0;
		} else {
			printf ("permalloc: allocating %zx bytes starting from %p"
				" at offset %tx\n",
				nbytes, ret_value, ret_value-stack_base);
			return ret_value;
		}
	} else {
		printf ("permalloc: allocating %zx bytes with malloc "
		        "(uninitialized mode)\n", nbytes);
		return malloc (nbytes);
	}
}

bool permalloc::is_from (void* p)
{
	char* pc = reinterpret_cast<char*> (p);
	return ((pc - stack_base >= 0) && (stack_top - pc > 0));
}

ptrdiff_t permalloc::get_offset (void* p)
{
	char* pc = reinterpret_cast<char*> (p);
	return (pc - stack_base);
}

