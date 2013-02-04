/**
 * TwetoConstMemory.cpp
 *
 *
 * @author : Claude Helmstetter, Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/03/02
 * @copyright : Verimag 2011
 */

#include "TwetoConstMemory.h"

#include <llvm/Type.h>
#include <llvm/DataLayout.h>
#include <llvm/DerivedTypes.h>
#include <iostream>

using namespace llvm;

void add_interval(map2i &values, intptr_t start, intptr_t size) {
  map2i::iterator i = values.lower_bound(start);
  map2i::iterator n;
  if (i==values.end()) {
    n = values.insert(pair2i(start,size)).first; // insert at end
  } else {
    if (i->first+i->second>=start) {
      i->second = std::max(i->second,start-i->first+size); // extend existing
      n = i;
    } else
      n = values.insert(pair2i(start,size)).first; // insert between
  }
  map2i::iterator s = values.lower_bound(n->first+n->second);
  n->second = std::max(n->second, s->first - n->first + s->second);
  values.erase(s,n);
}

bool is_member(map2i &values, intptr_t value) {
  map2i::iterator i = values.lower_bound(value);
  return i!=values.end() &&
    value < i->first+i->second;
}

map2i const_addresses = map2i();

void fill_const_addressses(Module *Mod, ExecutionEngine *EE) {
  static bool done = false;
  if (done) return;
  const Module::GlobalListType &globals = Mod->getGlobalList();
  Module::GlobalListType::const_iterator i = globals.begin();
  Module::GlobalListType::const_iterator ei = globals.end();
  for (;i!=ei;++i) {
    const GlobalVariable &gv = *i;
    if (gv.hasName() && !std::string(gv.getName()).compare(0,4,"_ZTV") &&
        gv.hasInitializer()) {
      size_t gv_size =
        EE->getDataLayout()->getTypeAllocSize(gv.getType()->getElementType());
      intptr_t gv_ptr = reinterpret_cast<intptr_t>(EE->getPointerToGlobal(&gv));
      // std::cerr <<"GlobalVariable: " <<std::string(gv.getName())
      //           <<' ' <<gv_ptr <<' ' <<gv_size <<'\n';
      add_interval(const_addresses, gv_ptr, gv_size);
    }
  }
  done = true;
}
