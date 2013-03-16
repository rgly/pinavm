/*
#include "SimpleBackendNameAllUsedStructsAndMergeFunctions.h"
#include "SimpleWriter.h"

void
SimpleBackendNameAllUsedStructsAndMergeFunctions::
getAnalysisUsage(AnalysisUsage & AU) const
{
	AU.addRequired < FindUsedTypes > ();
}

/// This method inserts names for any unnamed structure types that are used by
/// the program, and removes names from structure types that are not used by the
/// program.
///
bool SimpleBackendNameAllUsedStructsAndMergeFunctions::
runOnModule(Module & M)
{
	// Get a set of types that are used by the program...
	std::set < const Type *>UT =
	    getAnalysis < FindUsedTypes > ().getTypes();

	// Loop over the module symbol table, removing types from UT that are
	// already named, and removing names for types that are not used.
	//
	TypeSymbolTable & TST = M.getTypeSymbolTable();
	for (TypeSymbolTable::iterator TI = TST.begin(), TE = TST.end();
	     TI != TE;) {
		TypeSymbolTable::iterator I = TI++;

		// If this isn't a struct or array type, remove it from our set of types
		// to name. This simplifies emission later.
		if (!isa < StructType > (I->second)
		    && !isa < OpaqueType > (I->second)
		    && !isa < ArrayType > (I->second)) {
			TST.remove(I);
		} else {
			// If this is not used, remove it from the symbol table.
			std::set < const Type *>::iterator UTI =
			    UT.find(I->second);
			if (UTI == UT.end())
				TST.remove(I);
			else
				UT.erase(UTI);	// Only keep one name for this type.
		}
	}

	// UT now contains types that are not named.  Loop over it, naming
	// structure types.
	//
	bool Changed = false;
	unsigned RenameCounter = 0;
	for (std::set < const Type * >::const_iterator I = UT.begin(), E =
	     UT.end(); I != E; ++I)
		if (isa < StructType > (*I) || isa < ArrayType > (*I)) {
			while (M.
			       addTypeName("unnamed" +
					   utostr(RenameCounter), *I))
				++RenameCounter;
			Changed = true;
		}

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
*/
