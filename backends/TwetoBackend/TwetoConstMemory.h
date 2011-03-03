#ifndef TWETOCONSTMEMORY_HPP
#define TWETOCONSTMEMORY_HPP

#include <llvm/Module.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <map>

typedef std::map<intptr_t,intptr_t,std::greater<intptr_t> > map2i;
typedef std::pair<intptr_t,intptr_t> pair2i;

extern map2i const_addresses;

void add_interval(map2i &values, intptr_t start, intptr_t size);
bool is_member(map2i &values, intptr_t value);
void fill_const_addressses(llvm::Module*, llvm::ExecutionEngine*);

#endif // TWETOCONSTMEMORY_HPP
