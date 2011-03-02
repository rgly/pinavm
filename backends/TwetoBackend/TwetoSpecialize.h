#ifndef TWETOSPECIALIZE_HPP
#define TWETOSPECIALIZE_HPP

#include <llvm/Module.h>
#include <llvm/ExecutionEngine/JIT.h>

llvm::Function *tweto_specialize(llvm::ExecutionEngine*, llvm::Module*, llvm::Function*,
                                 llvm::Value **args_begin, llvm::Value **args_end);

void tweto_print_all_specialized_if_asked();
void tweto_print_all_specialized();

#endif // TWETOSPECIALIZE_HPP
