#include "SimpleBackendNameAllUsedStructsAndMergeFunctions.h"
#include "SimpleWriter.h"
#include "llvm/TypeFinder.h"
#include "llvm/ADT/SetVector.h"

#include <cassert>

namespace {
bool isContained(SetVector < Type *> * UT, Type * type)
{
	SetVector < Type *>::iterator UTI =
			std::find(UT->begin(), UT->end(), type);
	if (UTI == UT->end())
		return false;
	else
		return true;
}

// Simply remove named StructType from used type set. If an unused
// StructType has name, remove the name of the StructType.
SetVector < Type *> * removeNamedStructFromSet(
			SetVector < Type *> * UT, Module & M)
{
	TypeFinder TF;
	// Search for Named StructType over module.
	TF.run(M, true);
	std::vector < StructType *>::iterator it, eit;
	for ( it = TF.begin(), eit = TF.end(); it != eit; ++it) {
		Type* named_type = *it;

		if (isContained(UT, named_type) ) {
			UT->remove(named_type);
		} else {
			StructType* st = dyn_cast<StructType>(named_type);
			// delete the name if the type is not used.
			st->setName("");
		}
	}
	return UT;

}

bool nameUsedStruct(SetVector < Type *> * UT )
{
	bool Changed = false;
	unsigned RenameCounter = 0;
	SetVector < Type * >::const_iterator I, E;
	for (I = UT->begin(), E = UT->end(); I != E; ++I)
		if (isa < StructType > (*I) ) {
			StructType* st = dyn_cast<StructType>(*I);
			assert(! st->hasName());
			st->setName("unnamed" + utostr(RenameCounter));
			++RenameCounter;
			Changed = true;
		}

	return Changed;
}
} // end of unamed namespace.
void
SimpleBackendNameAllUsedStructsAndMergeFunctions::
getAnalysisUsage(AnalysisUsage & AU) const
{
	AU.addRequired < FindUsedTypes > ();
}


/// This method inserts names for any unnamed structure types that are used by
/// the program, 
///
bool SimpleBackendNameAllUsedStructsAndMergeFunctions::
runOnModule(Module & M)
{
	// Get a set of types that are used by the program...
	SetVector < Type *>UT = getAnalysis < FindUsedTypes > ().getTypes();
	SetVector < Type *> *UTP = &UT;

	UTP = removeNamedStructFromSet(UTP, M);

	// UTP now contains types that are not named.  Loop over it, naming
	// structure types.
	//
	bool Changed = nameUsedStruct(UTP);

	// Loop over all external functions and globals.  If we have two with
	// identical names, merge them.
	// FIXME: This code should disappear when we don't allow values with the same
	// names when they have different types!
	std::map < std::string, GlobalValue * >ExtSymbols;
	for (Module::iterator I = M.begin(), E = M.end(); I != E;) {
		Function *GV = I++;
		if (GV->isDeclaration() && GV->hasName()) {
			std::pair < std::map < std::string,
			    GlobalValue * >::iterator, bool > X =
			    ExtSymbols.
			    insert(std::make_pair(GV->getName(), GV));
			if (!X.second) {
				// Found a conflict, replace this global with the previous one.
				GlobalValue *OldGV = X.first->second;
				GV->replaceAllUsesWith(ConstantExpr::
						       getBitCast(OldGV,
								  GV->
								  getType
								  ()));
				GV->eraseFromParent();
				Changed = true;
			}
		}
	}
	// Do the same for globals.
	for (Module::global_iterator I = M.global_begin(), E =
	     M.global_end(); I != E;) {
		GlobalVariable *GV = I++;
		if (GV->isDeclaration() && GV->hasName()) {
			std::pair < std::map < std::string,
			    GlobalValue * >::iterator, bool > X =
			    ExtSymbols.
			    insert(std::make_pair(GV->getName(), GV));
			if (!X.second) {
				// Found a conflict, replace this global with the previous one.
				GlobalValue *OldGV = X.first->second;
				GV->replaceAllUsesWith(ConstantExpr::
						       getBitCast(OldGV,
								  GV->
								  getType
								  ()));
				GV->eraseFromParent();
				Changed = true;
			}
		}
	}

	return Changed;
}

const char *SimpleBackendNameAllUsedStructsAndMergeFunctions::getPassName() const
{
	return "Simple backend type canonicalizer";
}

char SimpleBackendNameAllUsedStructsAndMergeFunctions::ID = 0;
