#include <string>
#include <map>
#include <cstdio>

#include "llvm/GlobalValue.h"
#include "llvm/CallingConv.h"
#include "llvm/DerivedTypes.h"
#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/LLVMContext.h"
#include "llvm/ADT/SmallString.h"

#include "Port.hpp"
#include "Channel.hpp"
#include "SimpleChannel.hpp"
#include "SCElab.h"
#include "PromelaWriter.h"
#include "Process.hpp"
#include "IRModule.hpp"
#include "SCJit.hpp"
#include "ALLConstruct.h"
#include "SCCFactory.hpp"

#include "utils.h"
#include "config.h"

using namespace std;

static Function* currentFunction;
static Instruction* pointerToInst;
static Process* currentProcess;
bool fieldPrintedCheck=false;
/***************************************************************************
 *************** Static functions ******************************************
 **************************************************************************/

// isInlineAsm - Check if the instruction is a call to an inline asm chunk
static bool isInlineAsm(const Instruction & I)
{
	if (isa < CallInst > (&I) && isa < InlineAsm > (I.getOperand(0)))
		return true;
	return false;
}

// isInlinableInst - Attempt to inline instructions into their uses to build
// trees as much as possible.  To do this, we have to consistently decide
// what is acceptable to inline, so that variable declarations don't get
// printed and an extra copy of the expr is not emitted.
//
static bool isInlinableInst(const Instruction & I)
{
	// Always inline cmp instructions, even if they are shared by multiple
	// expressions.  GCC generates horrible code if we don't.
	if (isa < CmpInst > (I))
		return true;

	// Must be an expression, must be used exactly once.  If it is dead, we
	// emit it inline where it would go.
	if (I.getType() == Type::getVoidTy(I.getContext())
		|| !I.hasOneUse() || isa < TerminatorInst > (I)
		|| isa < CallInst > (I) || isa < PHINode > (I)
		|| isa < LoadInst > (I) || isa < VAArgInst > (I)
		|| isa < InsertElementInst > (I)
		|| isa < InsertValueInst > (I))
		// Don't inline a load across a store or other bad things!
		return false;

	// Must not be used in inline asm, extractelement, or shufflevector.
	if (I.hasOneUse()) {
		const Instruction & User =
			cast < Instruction > (*I.use_back());
		if (isInlineAsm(User) || isa < ExtractElementInst > (User)
			|| isa < ShuffleVectorInst > (User))
			return false;
	}
	// Only inline instruction it if it's use is in the same BB as the inst.
	return I.getParent() == cast < Instruction >
		(I.use_back())->getParent();
}

// isDirectAlloca - Define fixed sized allocas in the entry block as direct
// variables which are accessed with the & operator.  This causes GCC to
// generate significantly better code than to emit alloca calls directly.
//
static const AllocaInst *isDirectAlloca(const Value * V)
{
	const AllocaInst *AI = dyn_cast < AllocaInst > (V);
	if (!AI)
		return false;
	if (AI->isArrayAllocation())
		return 0;	// FIXME: we can also inline fixed size array allocas!
	if (AI->getParent() !=
		&AI->getParent()->getParent()->getEntryBlock())
		return 0;
	return AI;
}

/***************************************************************************
 ******************* Constructors ******************************************
 **************************************************************************/

PromelaWriter::PromelaWriter(Frontend* fe, formatted_raw_ostream &o, bool encodeEventsAsBool, bool useRelativeClocks, bool bug)
	: ModulePass(ID), Out(o), IL(0), Mang(0), 
	  TheModule(0), TAsm(0), TD(0), OpaqueCounter(0), NextAnonValueNumber(0) {
	FPCounter = 0;
	this->sccfactory = fe->getConstructs();
	this->elab = fe->getElab();
	this->scjit = fe->getJit();
	this->relativeClocks = useRelativeClocks;
	this->eventsAsBool = encodeEventsAsBool;
	this->insertBug = bug;
}

PromelaWriter::PromelaWriter(formatted_raw_ostream &o)
	: ModulePass(ID), Out(o), IL(0), Mang(0), 
	  TheModule(0), TAsm(0), TD(0), OpaqueCounter(0), NextAnonValueNumber(0) {
	FPCounter = 0;
}


/***************************************************************************
 **************** Print functions ******************************************
 **************************************************************************/

/// printStructReturnPointerFunctionType - This is like printType for a struct
/// return type, except, instead of printing the type as void (*)(Struct*, ...)
/// print it as "Struct (*)(...)", for struct return functions.
void PromelaWriter::
printStructReturnPointerFunctionType(formatted_raw_ostream & Out,
				const AttrListPtr & PAL,
				PointerType * TheTy)
{
	FunctionType *FTy = cast < FunctionType > (TheTy->getElementType());
	std::stringstream FunctionInnards;
	FunctionInnards << " (*) (";
	bool PrintedType = false;

	FunctionType::param_iterator I = FTy->param_begin(), E =
		FTy->param_end();
	Type *RetTy =
		cast < PointerType > (*I)->getElementType();
	unsigned Idx = 1;
	for (++I, ++Idx; I != E; ++I, ++Idx) {
		if (PrintedType)
			FunctionInnards << ", ";
		Type *ArgTy = *I;
		if (PAL.paramHasAttr(Idx, this->getAttributes(Attributes::ByVal))) {
			assert(isa < PointerType > (ArgTy));
			ArgTy =	cast < PointerType > (ArgTy)->getElementType();
		}
		printType(FunctionInnards, ArgTy,
			/*isSigned= */ PAL.paramHasAttr(Idx,
							this->getAttributes(Attributes::SExt)),
			"");
		PrintedType = true;
	}
	if (FTy->isVarArg()) {
		if (PrintedType)
			FunctionInnards << ", ...";
	} else if (!PrintedType) {
		FunctionInnards << "void";
	}
	FunctionInnards << ')';
	std::string tstr = FunctionInnards.str();


	printType(Out, RetTy,
		/*isSigned= */ PAL.paramHasAttr(0, this->getAttributes(Attributes::SExt)),
		tstr);
}

raw_ostream &
PromelaWriter::printSimpleType(formatted_raw_ostream & Out,
			Type * Ty, bool isSigned,
			const std::string & NameSoFar)
{
	assert((Ty->isPrimitiveType() || Ty->isIntegerTy()
			|| isa < VectorType > (Ty))
		&& "Invalid type for printSimpleType");

	switch (Ty->getTypeID()) {
	case Type::VoidTyID:
		return Out << "void ";
	case Type::IntegerTyID:{
		unsigned NumBits = cast < IntegerType > (Ty)->getBitWidth();
		fieldPrintedCheck=true;
		if (NumBits == 1)
			return Out << "bit " + NameSoFar;
		else if (NumBits <= 8)
			return Out << "byte " + NameSoFar;
		else if (NumBits <= 16)
			return Out << "short " + NameSoFar;
		else
			return Out << "int " + NameSoFar;
	}
	case Type::FloatTyID:
		triggerError(Out, "NYI : float type");
	case Type::DoubleTyID:
		triggerError(Out, "NYI : double type");
	case Type::X86_FP80TyID:
	case Type::PPC_FP128TyID:
	case Type::FP128TyID:
		triggerError(Out, "NYI : long type");
	case Type::VectorTyID:{
		triggerError(Out, "NYI : Vector type");
		VectorType *VTy = cast < VectorType > (Ty);
		printSimpleType(Out, VTy->getElementType(),
				isSigned,
				NameSoFar + "[" + utostr(TD->getTypeAllocSize(VTy))+ "]");		
	}

	default:
#ifndef NDEBUG
		errs() << "Unknown primitive type: " << *Ty << "\n";
#endif
		llvm_unreachable(0);
	}
}

std::ostream &
PromelaWriter::printSimpleType(std::ostream & Out, Type * Ty,
			bool isSigned,
			const std::string & NameSoFar)
{
	assert((Ty->isPrimitiveType() || Ty->isIntegerTy()
			|| isa < VectorType > (Ty))
		&& "Invalid type for printSimpleType");

	if (NameSoFar != "")
		Out << NameSoFar << " : ";

	switch (Ty->getTypeID()) {
	case Type::VoidTyID:
		return Out << "void ";
	case Type::IntegerTyID:{
		unsigned NumBits =
			cast < IntegerType > (Ty)->getBitWidth();
		return Out << (isSigned ? "sint" : "uint") << "["
			   << NumBits << "]";
	}
	case Type::FloatTyID:
		return Out << "float ";
	case Type::DoubleTyID:
		return Out << "double ";
		// Lacking emulation of FP80 on PPC, etc., we assume whichever of these is
		// present matches host 'long double'.
	case Type::X86_FP80TyID:
	case Type::PPC_FP128TyID:
	case Type::FP128TyID:
		return Out << "long double ";

	case Type::VectorTyID:{
		triggerError(this->Out, "NYI : Vector type");
		VectorType *VTy = cast < VectorType > (Ty);
		return printSimpleType(Out, VTy->getElementType(),
				isSigned,
				" __attribute__((vector_size("
				+
				utostr(TD->getTypeAllocSize(VTy)) + " ))) ");
	}

	default:
#ifndef NDEBUG
		errs() << "Unknown primitive type: " << *Ty << "\n";
#endif
		llvm_unreachable(0);
	}
}

// Pass the Type* and the variable name and this prints out the variable
// declaration.
//
raw_ostream &
PromelaWriter::printType(formatted_raw_ostream & Out,
			Type * Ty,
			bool isSigned, const std::string & NameSoFar,
			bool IgnoreName, const AttrListPtr & PAL)
{	
 	while (isa<PointerType>(Ty)) {
		//Out<<"Inside While Pointer \n";
 		Ty = cast<PointerType>(Ty)->getElementType();
 	}	

	if (Ty->isPrimitiveType() || Ty->isIntegerTy() || isa < VectorType > (Ty)) {
		if (Ty->isIntegerTy())
			TRACE_5("WHY AM I NOT HERE AS YET ------------------------------->************"<<NameSoFar);
		//Out<<"Inside simple type \n";
		printSimpleType(Out, Ty, isSigned, NameSoFar);			
		return Out;
	}
	// Check to see if the type is named.
	if (!IgnoreName ) {
		//Out<<"Ignore Name Or Opaque Type \n";
		std::map < Type *, std::string >::iterator ITN = TypeNames.find(Ty), ETN = TypeNames.end();
		if (isSystemCType(Ty)){
			//Out<<"Inside system C type \n";
			return Out;
		}
		if (isSystemCStruct(dyn_cast<StructType>(Ty)))
			if (ITN!=ETN){
			std::string tName = ITN->second;
			if (tName.find("struct_sc_dt::sc_uint")!=string::npos)
			return Out<<"int "<<NameSoFar;
			else
			return Out<<"CHECK";}
		
		while (isa<PointerType>(Ty)) {
			//Out<<"Deep Inside Pointer Types  \n";
			Ty = cast<PointerType>(Ty)->getElementType();
		}
		//std::map < Type *, std::string >::iterator ITN = TypeNames.find(Ty), ETN = TypeNames.end();
		if (ITN != ETN) {
			//Out<<"Deep Inside ITN!=ETN \n";
			std::string tName = ITN->second;
			if (tName!="")
			//if (tName.find("struct_sc_dt::sc_uint<8>")!=string::npos)
			return Out << tName << " " << NameSoFar;
			else {
				fieldPrintedCheck=false;
				return Out;
			     }
		}
	}

	switch (Ty->getTypeID()) {
	case Type::FunctionTyID:{
		/*ErrorMsg << "NYI : use of complex type : FunctionTy : " << NameSoFar;
		triggerError(Out);*/

		/*const FunctionType *FTy =
			cast < FunctionType > (Ty);
		std::stringstream FunctionInnards;
		FunctionInnards << " (" << NameSoFar << ") (";
		unsigned Idx = 1;
		for (FunctionType::param_iterator I =
			     FTy->param_begin(), E = FTy->param_end();
		     I != E; ++I) {
			Type *ArgTy = *I;
			if (PAL.paramHasAttr(Idx, Attribute::ByVal)) {
				assert(isa < PointerType >(ArgTy));
				ArgTy =	cast < PointerType > (ArgTy)->getElementType();
			}
			if (I != FTy->param_begin())
				FunctionInnards << ", ";
			printType(FunctionInnards, ArgTy, PAL.paramHasAttr(Idx, Attribute::SExt), "");
			++Idx;
		}
		if (FTy->isVarArg()) {
			if (FTy->getNumParams())
				FunctionInnards << ", ...";
		} else if (!FTy->getNumParams()) {
			FunctionInnards << "void";
		}
		FunctionInnards << ')';
		std::string tstr = FunctionInnards.str();
		printType(Out, FTy->getReturnType(), PAL.paramHasAttr(0, Attribute::SExt), tstr);
		return Out;*/
		return Out;
	}
	case Type::StructTyID:{
		StructType *STy = cast < StructType > (Ty);

		// if a OpaqueType
		if (STy->isOpaque() ) {
			ErrorMsg <<"NYI : use of complex type : OpaqueTy : "
				 <<	NameSoFar;
			triggerError(Out);

			std::string TyName = "struct opaque_" 
						+ itostr(OpaqueCounter++);
			assert(TypeNames.find(Ty) == TypeNames.end());
			insertTypeName(Ty, TyName);
			return Out << TyName << ' ' << NameSoFar;
		}

		TRACE_4("/**** Handling StructTy type in printType() ****/\n");
		Out << NameSoFar + " {\n";
		unsigned Idx = 0;
		bool fieldPrinted = false;
		//bool flag=false;
		//Out<<"Inside struct type \n";
		for (StructType::element_iterator I = STy->element_begin(), E = STy->element_end(); I != E; ++I) {
			TRACE_4("\n/**** Dumping struct element in printType() ****/\n");
			if (! isSystemCType(*I)) {
				//flag=false;
				if (fieldPrinted||fieldPrintedCheck)
					Out << ";\n";
				else
					Out << " ";
				//Out<<(*I);
				printType(Out, *I, false, "field" + utostr(Idx));
				fieldPrinted = true;
			}
			Idx++;
		}
		Out<<";";
		fieldPrintedCheck=false;
		TRACE_4("\n/**** struct elements printed ****/\n");
		Out << '\n';
		Out << '}';
		return Out;
	}

	case Type::PointerTyID:{
		PointerType *PTy = cast < PointerType > (Ty);
		std::string ptrName = NameSoFar;

		TRACE_4("/**** Handling PointerTy type in printType() ****/\n");
		Out<<"Inside PointerTypeID type \n";
		if (isa < ArrayType > (PTy->getElementType()) || isa < VectorType > (PTy->getElementType()))
			ptrName = "(" + ptrName + ")";

		if (!PAL.isEmpty())
			// Must be a function ptr cast!
			return printType(Out, PTy->getElementType(), false, ptrName, true, PAL);
		return printType(Out, PTy->getElementType(), false, ptrName);
	}

	case Type::ArrayTyID:{
		ErrorMsg <<
			"NYI : use of complex type : ArrayTy : " <<
			NameSoFar;
		triggerError(Out);
		//Out<<"Inside ArrayTypeID type \n";
		const ArrayType *ATy = cast < ArrayType > (Ty);
		unsigned NumElements = ATy->getNumElements();
		if (NumElements == 0)
			NumElements = 1;
		// Arrays are wrapped in structs to allow them to have normal
		// value semantics (avoiding the array "decay").
		//Out << NameSoFar << " { ";
		printType(Out, ATy->getElementType(), false,
			NameSoFar+"[" + utostr(NumElements) + "]");
		//return Out << "; }";
	}

	default:
		Ty->dump();
		llvm_unreachable("Unhandled case in getTypeProps!");
	}

	return Out;
}


// Pass the Type* and the variable name and this prints out the variable
// declaration.
//
std::ostream &
PromelaWriter::printType(std::ostream & Out,
			Type * Ty,
			bool isSigned, const std::string & NameSoFar,
			bool IgnoreName, const AttrListPtr & PAL)
{
 	while (isa<PointerType>(Ty)) {
 		Ty = cast<PointerType>(Ty)->getElementType();
 	}	

	if (Ty->isPrimitiveType() || Ty->isIntegerTy()
		|| isa < VectorType > (Ty)) {
		printSimpleType(Out, Ty, isSigned, NameSoFar);
		return Out;
	}

	// Check whether Ty is a OpaqueTy
	bool IsOpaque = false;
	if (isa<StructType>(Ty)){
		StructType* STy = dyn_cast<StructType>(Ty);
		IsOpaque = STy->isOpaque();
	}

	// Check to see if the type is named.
	if (!IgnoreName || IsOpaque ) {
		if (isSystemCType(Ty))/*{
			std::map < Type *, std::string >::iterator ITN = TypeNames.find(Ty), ETN = TypeNames.end();
		        if (ITN != ETN) {
			std::string tName = ITN->second;
			if (tName.find("struct_sc_dt::sc_uint")!=string::npos)
			 	return Out << "int" << " " << NameSoFar;}*/
			return Out;
		//}
		while (isa<PointerType>(Ty)) {
			Ty = cast<PointerType>(Ty)->getElementType();
		}

 		std::map < Type *, std::string >::iterator I = TypeNames.find(Ty);
 		if (I != TypeNames.end()) {
			std::string tName = I->second;
			return Out << tName << " " << NameSoFar;
		}
 	}

	switch (Ty->getTypeID()) {
	case Type::FunctionTyID:{
		ErrorMsg << "NYI : use of complex type : FunctionTy : " << NameSoFar;
		triggerError(this->Out);
		
		FunctionType *FTy =
			cast < FunctionType > (Ty);
		std::stringstream FunctionInnards;
		FunctionInnards << " (" << NameSoFar << ") (";
		unsigned Idx = 1;
		for (FunctionType::param_iterator I =
			     FTy->param_begin(), E = FTy->param_end();
		     I != E; ++I) {
			Type *ArgTy = *I;
			if (PAL.
				paramHasAttr(Idx, this->getAttributes(Attributes::ByVal))) {
				assert(isa < PointerType >
					(ArgTy));
				ArgTy =
					cast < PointerType >
					(ArgTy)->getElementType();
			}
			if (I != FTy->param_begin())
				FunctionInnards << ", ";
			printType(FunctionInnards, ArgTy,
				/*isSigned= */
				PAL.paramHasAttr(Idx,
						this->getAttributes(Attributes::SExt)), "");
			++Idx;
		}
		if (FTy->isVarArg()) {
			if (FTy->getNumParams())
				FunctionInnards << ", ...";
		} else if (!FTy->getNumParams()) {
			FunctionInnards << "void";
		}
		FunctionInnards << ')';
		std::string tstr = FunctionInnards.str();
		printType(Out, FTy->getReturnType(),
			/*isSigned= */ PAL.paramHasAttr(0,
							this->getAttributes(Attributes::SExt)),
			tstr);
		return Out;
	}
	case Type::StructTyID:{
		StructType *STy = cast < StructType > (Ty);

		//if a OpaqueType
		if (STy->isOpaque()) {
			ErrorMsg << "NYI : use of complex type : OpaqueTy : "
				 << NameSoFar;
			triggerError(this->Out);

			std::string TyName =
				"struct opaque_" + itostr(OpaqueCounter++);
			assert(TypeNames.find(Ty) == TypeNames.end());
			insertTypeName(Ty, TyName);
			return Out << TyName << ' ' << NameSoFar;

		}

		TRACE_4("/**** Handling StructTy type in printType() ****/\n");
		Out << NameSoFar + " {\n";
		unsigned Idx = 0;
		//bool fieldPrinted = false;
		for (StructType::element_iterator I = STy->element_begin(), E = STy->element_end(); I != E; ++I) {
			TRACE_4("\n/**** Dumping struct element in printType() ****/\n");
			if (! isSystemCType(*I)) {

				if (fieldPrintedCheck)
					Out << ";\n";
				else
					Out << " ";
				//Out<<(*I);
				printType(Out, *I, false, "field" + utostr(Idx));
				//fieldPrinted = true;
			}
			Idx++;
		}
		Out<<";";
		fieldPrintedCheck=false;
		TRACE_4("\n/**** struct elements printed ****/\n");

		Out << '}';
		return Out;
	}

	case Type::PointerTyID:{
		PointerType *PTy = cast < PointerType > (Ty);
		std::string ptrName = "*" + NameSoFar;

		TRACE_4("/**** Handling PointerTy type in printType() ****/\n");

		if (isa < ArrayType > (PTy->getElementType()) ||
			isa < VectorType > (PTy->getElementType()))
			ptrName = "(" + ptrName + ")";

		if (!PAL.isEmpty())
			// Must be a function ptr cast!
			return printType(Out, PTy->getElementType(), false, ptrName, true, PAL);
		return printType(Out, PTy->getElementType(), false, ptrName);
	}

	case Type::ArrayTyID:{
		ErrorMsg <<
			"NYI : use of complex type : ArrayTy : " <<
			NameSoFar;
		triggerError(this->Out);

		const ArrayType *ATy = cast < ArrayType > (Ty);
		unsigned NumElements = ATy->getNumElements();
		if (NumElements == 0)
			NumElements = 1;
		// Arrays are wrapped in structs to allow them to have normal
		// value semantics (avoiding the array "decay").
		Out << NameSoFar << " { ";
		printType(Out, ATy->getElementType(), false,
			"array[" + utostr(NumElements) + "]");
		return Out << "; }";
	}

	default:
		llvm_unreachable("Unhandled case in getTypeProps!");
	}

	return Out;
}
/*raw_ostream &printInitialValue(formatted_raw_ostream &Out,
			Type * Ty,
			bool isSigned, const std::string & NameSoFar)
{
assert((Ty->isPrimitiveType() || Ty->isIntegerTy()
			|| isa < VectorType > (Ty))
		&& "Invalid type for printInitialValue");

	switch (Ty->getTypeID()) {
	case Type::VoidTyID:
		return Out << NameSoFar<<" = NULL";
	case Type::IntegerTyID:{
		unsigned NumBits = cast < IntegerType > (Ty)->getBitWidth();
		if (NumBits == 1)
			return Out <<NameSoFar<<" = true";
		else if (NumBits <= 8)
			return Out << NameSoFar<< " = 0";
		else if (NumBits <= 16)
			return Out << NameSoFar<< " = 0";
		else
			return Out << NameSoFar<< " = 0";
	}
	case Type::FloatTyID:
		triggerError(Out, "NYI : float type");
	case Type::DoubleTyID:
		triggerError(Out, "NYI : double type");
	case Type::X86_FP80TyID:
	case Type::PPC_FP128TyID:
	case Type::FP128TyID:
		triggerError(Out, "NYI : long type");
	case Type::VectorTyID:{
		triggerError(Out, "NYI : Vector type");
		const VectorType *VTy = cast < VectorType > (Ty);
		printInitialValue(Out, VTy->getElementType(),
				isSigned,
				NameSoFar + "[" + utostr(TD->getTypeAllocSize(VTy))+ "]");	
	}

	default:
#ifndef NDEBUG
		errs() << "Unknown primitive type: " << *Ty << "\n";
#endif
		llvm_unreachable(0);
	}
}*/	

void
PromelaWriter::printConstantArray(ConstantArray * CPA, bool Static)
{

	// As a special case, print the array as a string if it is an array of
	// ubytes or an array of sbytes with positive values.
	//
	Type *ETy = CPA->getType()->getElementType();
	bool isString = (ETy == Type::getInt8Ty(CPA->getContext()) ||
			ETy == Type::getInt8Ty(CPA->getContext()));

	// Make sure the last character is a null char, as automatically added by C
	if (isString && (CPA->getNumOperands() == 0 ||
				!cast < Constant >
				(*(CPA->op_end() - 1))->isNullValue()))
		isString = false;

	if (isString) {
		Out << '\"';
		// Keep track of whether the last number was a hexadecimal escape
		bool LastWasHex = false;

		// Do not include the last character, which we know is null
		for (unsigned i = 0, e = CPA->getNumOperands() - 1; i != e;
		     ++i) {
			unsigned char C =
				cast < ConstantInt >
				(CPA->getOperand(i))->getZExtValue();

			// Print it out literally if it is a printable character.  The only thing
			// to be careful about is when the last letter output was a hex escape
			// code, in which case we have to be careful not to print out hex digits
			// explicitly (the C compiler thinks it is a continuation of the previous
			// character, sheesh...)
			//
			if (isprint(C) && (!LastWasHex || !isxdigit(C))) {
				LastWasHex = false;
				if (C == '"' || C == '\\')
					Out << "\\" << (char) C;
				else
					Out << (char) C;
			} else {
				LastWasHex = false;
				switch (C) {
				case '\n':
					Out << "\\n";
					break;
				case '\t':
					Out << "\\t";
					break;
				case '\r':
					Out << "\\r";
					break;
				case '\v':
					Out << "\\v";
					break;
				case '\a':
					Out << "\\a";
					break;
				case '\"':
					Out << "\\\"";
					break;
				case '\'':
					Out << "\\\'";
					break;
				default:
					Out << "\\x";
					Out << (char) ((C / 16 < 10)
						? (C / 16 +
							'0') : (C / 16 -
								10 +
								'A'));
					Out << (char) (((C & 15) < 10)
						? ((C & 15) +
							'0') : ((C & 15)
								- 10 +
								'A'));
					LastWasHex = true;
					break;
				}
			}
		}
		Out << '\"';
	} else {
		Out << '{';
		if (CPA->getNumOperands()) {
			Out << ' ';
			printConstant(cast < Constant >
				(CPA->getOperand(0)), Static);
			for (unsigned i = 1, e = CPA->getNumOperands();
			     i != e; ++i) {
				Out << ", ";
				printConstant(cast < Constant >
					(CPA->getOperand(i)),
					Static);
			}
		}
		Out << " }";
	}
}

void PromelaWriter::printConstantVector(ConstantVector * CP, bool Static)
{
	Out << '{';
	if (CP->getNumOperands()) {
		Out << ' ';
		printConstant(cast < Constant > (CP->getOperand(0)),
			Static);
		for (unsigned i = 1, e = CP->getNumOperands(); i != e; ++i) {
			Out << ", ";
			printConstant(cast < Constant >
				(CP->getOperand(i)), Static);
		}
	}
	Out << " }";
}

// isFPCSafeToPrint - Returns true if we may assume that CFP may be written out
// textually as a double (rather than as a reference to a stack-allocated
// variable). We decide this by converting CFP to a string and back into a
// double, and then checking whether the conversion results in a bit-equal
// double to the original value of CFP. This depends on us and the target C
// compiler agreeing on the conversion process (which is pretty likely since we
// only deal in IEEE FP).
//
static bool isFPCSafeToPrint(const ConstantFP * CFP)
{
	bool ignored;
	// Do long doubles in hex for now.
	if (CFP->getType() != Type::getFloatTy(CFP->getContext()) &&
		CFP->getType() != Type::getDoubleTy(CFP->getContext()))
		return false;
	APFloat APF = APFloat(CFP->getValueAPF());	// copy
	if (CFP->getType() == Type::getFloatTy(CFP->getContext()))
		APF.convert(APFloat::IEEEdouble,
			APFloat::rmNearestTiesToEven, &ignored);
#if HAVE_PRINTF_A && ENABLE_CBE_PRINTF_A
	char Buffer[100];
	sprintf(Buffer, "%a", APF.convertToDouble());
	if (!strncmp(Buffer, "0x", 2) ||
		!strncmp(Buffer, "-0x", 3) || !strncmp(Buffer, "+0x", 3))
		return APF.bitwiseIsEqual(APFloat(atof(Buffer)));
	return false;
#else
	SmallString<10> smallstr;
	// parameter : FormatPrecision, FormatMaxPadding 
	// use scientific notation here due to MaxPadding=0
	APF.toString(smallstr, 5, 0);
	std::string StrVal = smallstr.str().str(); // ->StringRef -> std::string

	while (StrVal[0] == ' ')
		StrVal.erase(StrVal.begin());

	// Check to make sure that the stringized number is not some string like "Inf"
	// or NaN.  Check that the string matches the "[-+]?[0-9]" regex.
	if ((StrVal[0] >= '0' && StrVal[0] <= '9') ||
		((StrVal[0] == '-' || StrVal[0] == '+') &&
			(StrVal[1] >= '0' && StrVal[1] <= '9')))
		// Reparse stringized version!
		return APF.bitwiseIsEqual(APFloat(atof(StrVal.c_str())));
	return false;
#endif
}

// printConstant - The LLVM Constant to Simple Constant converter.
void PromelaWriter::printConstant(Constant * CPV, bool Static)
{
	TRACE_4("/**** Printing constant ****/\n");

	if (const ConstantExpr * CE = dyn_cast < ConstantExpr > (CPV)) {
		switch (CE->getOpcode()) {
		case Instruction::Trunc:
		case Instruction::ZExt:
		case Instruction::SExt:
		case Instruction::FPTrunc:
		case Instruction::FPExt:
		case Instruction::UIToFP:
		case Instruction::SIToFP:
		case Instruction::FPToUI:
		case Instruction::FPToSI:
		case Instruction::PtrToInt:
		case Instruction::IntToPtr:
		case Instruction::BitCast:
			Out << "(";
			if (CE->getOpcode() == Instruction::SExt
				&& CE->getOperand(0)->getType() ==
				Type::getInt1Ty(CPV->getContext())) {
				// Make sure we really sext from bool here by subtracting from 0
				Out << "0-";
			}
			printConstant(CE->getOperand(0), Static);
			if (CE->getType() ==
				Type::getInt1Ty(CPV->getContext())
				&& (CE->getOpcode() == Instruction::Trunc
					|| CE->getOpcode() == Instruction::FPToUI
					|| CE->getOpcode() == Instruction::FPToSI
					|| CE->getOpcode() ==
					Instruction::PtrToInt)) {
				// Make sure we really truncate to bool here by anding with 1
				Out << "&1u";
			}
			Out << ')';
			return;

		case Instruction::GetElementPtr:
			//Out << "(";
			printGEPExpression(CE->getOperand(0),
					gep_type_begin(CPV),
					gep_type_end(CPV), Static);
			//Out << ")";
			return;
		case Instruction::Select:
			printConstant(CE->getOperand(0), Static);
			Out << '?';
			printConstant(CE->getOperand(1), Static);
			Out << ':';
			printConstant(CE->getOperand(2), Static);
			Out << ')';
			return;
		case Instruction::Add:
		case Instruction::FAdd:
		case Instruction::Sub:
		case Instruction::FSub:
		case Instruction::Mul:
		case Instruction::FMul:
		case Instruction::SDiv:
		case Instruction::UDiv:
		case Instruction::FDiv:
		case Instruction::URem:
		case Instruction::SRem:
		case Instruction::FRem:
		case Instruction::And:
		case Instruction::Or:
		case Instruction::Xor:
		case Instruction::ICmp:
		case Instruction::Shl:
		case Instruction::LShr:
		case Instruction::AShr:
		{
			Out << '(';
			printConstant(CE->getOperand(0), CE->getOpcode());
			switch (CE->getOpcode()) {
			case Instruction::Add:
			case Instruction::FAdd:
				Out << " + ";
				break;
			case Instruction::Sub:
			case Instruction::FSub:
				Out << " - ";
				break;
			case Instruction::Mul:
			case Instruction::FMul:
				Out << " * ";
				break;
			case Instruction::URem:
			case Instruction::SRem:
			case Instruction::FRem:
				Out << " % ";
				break;
			case Instruction::UDiv:
			case Instruction::SDiv:
			case Instruction::FDiv:
				Out << " / ";
				break;
			case Instruction::And:
				Out << " & ";
				break;
			case Instruction::Or:
				Out << " | ";
				break;
			case Instruction::Xor:
				Out << " ^ ";
				break;
			case Instruction::Shl:
				Out << " << ";
				break;
			case Instruction::LShr:
			case Instruction::AShr:
				Out << " >> ";
				break;
			case Instruction::ICmp:
				switch (CE->getPredicate()) {
				case ICmpInst::ICMP_EQ:
					Out << " == ";
					break;
				case ICmpInst::ICMP_NE:
					Out << " != ";
					break;
				case ICmpInst::ICMP_SLT:
				case ICmpInst::ICMP_ULT:
					Out << " < ";
					break;
				case ICmpInst::ICMP_SLE:
				case ICmpInst::ICMP_ULE:
					Out << " <= ";
					break;
				case ICmpInst::ICMP_SGT:
				case ICmpInst::ICMP_UGT:
					Out << " > ";
					break;
				case ICmpInst::ICMP_SGE:
				case ICmpInst::ICMP_UGE:
					Out << " >= ";
					break;
				default:
					llvm_unreachable
						("Illegal ICmp predicate");
				}
				break;
			default:
				llvm_unreachable
					("Illegal opcode here!");
			}
			printConstant(CE->getOperand(1),
					CE->getOpcode());
			Out << ')';
			return;
		}
		case Instruction::FCmp:{
			Out << '(';
			if (CE->getPredicate() ==
				FCmpInst::FCMP_FALSE)
				Out << "0";
			else if (CE->getPredicate() ==
				FCmpInst::FCMP_TRUE)
				Out << "1";
			else {
				const char *op = 0;
				switch (CE->getPredicate()) {
				default:
					llvm_unreachable
						("Illegal FCmp predicate");
				case FCmpInst::FCMP_ORD:
					op = "ord";
					break;
				case FCmpInst::FCMP_UNO:
					op = "uno";
					break;
				case FCmpInst::FCMP_UEQ:
					op = "ueq";
					break;
				case FCmpInst::FCMP_UNE:
					op = "une";
					break;
				case FCmpInst::FCMP_ULT:
					op = "ult";
					break;
				case FCmpInst::FCMP_ULE:
					op = "ule";
					break;
				case FCmpInst::FCMP_UGT:
					op = "ugt";
					break;
				case FCmpInst::FCMP_UGE:
					op = "uge";
					break;
				case FCmpInst::FCMP_OEQ:
					op = "oeq";
					break;
				case FCmpInst::FCMP_ONE:
					op = "one";
					break;
				case FCmpInst::FCMP_OLT:
					op = "olt";
					break;
				case FCmpInst::FCMP_OLE:
					op = "ole";
					break;
				case FCmpInst::FCMP_OGT:
					op = "ogt";
					break;
				case FCmpInst::FCMP_OGE:
					op = "oge";
					break;
				}
				Out << "llvm_fcmp_" << op << "(";
				printConstant(CE->getOperand(0), CE->getOpcode());
				Out << ", ";
				printConstant(CE->getOperand(1), CE->getOpcode());
				Out << ")";
			}
			Out << ')';
			return;
		}
		default:
#ifndef NDEBUG
			errs() <<
				"PromelaWriter Error: Unhandled constant expression: "
			       << *CE << "\n";
#endif
			llvm_unreachable(0);
		}
	} else if (isa < UndefValue > (CPV)
		&& CPV->getType()->isSingleValueType()) {
		Out << "((";
		printType(Out, CPV->getType());	// sign doesn't matter
		Out << ")/*UNDEF*/";
		if (!isa < VectorType > (CPV->getType())) {
			Out << "0)";
		} else {
			Out << "{})";
		}
		return;
	}

	if (ConstantInt * CI = dyn_cast < ConstantInt > (CPV)) {
		Type *Ty = CI->getType();
		if (Ty == Type::getInt1Ty(CPV->getContext()))
			Out << (CI->getZExtValue()? '1' : '0');
		else if (Ty == Type::getInt32Ty(CPV->getContext()))
			Out << CI->getZExtValue();
		else if (Ty->getPrimitiveSizeInBits() > 32)
			Out << CI->getZExtValue();// << "ull";
		else {
			//Out << "((";
			//printSimpleType(Out, Ty, false) << ')';
			if (CI->isMinValue(true))
				Out << CI->getZExtValue();
			else
				Out << CI->getSExtValue();
			//Out << ')';
		}
		return;
	}

	switch (CPV->getType()->getTypeID()) {
	case Type::FloatTyID:
	case Type::DoubleTyID:
	case Type::X86_FP80TyID:
	case Type::PPC_FP128TyID:
	case Type::FP128TyID:{
		ConstantFP *FPC = cast < ConstantFP > (CPV);
		std::map < const ConstantFP *,
				unsigned >::iterator I =
			FPConstantMap.find(FPC);
		if (I != FPConstantMap.end()) {
			// Because of FP precision problems we must load from a stack allocated
			// value that holds the value in hex.
			Out << "(*(" << (FPC->getType() ==
					Type::getFloatTy(CPV->
							getContext
							())?
					"float" : FPC->
					getType() ==
					Type::getDoubleTy(CPV->
							getContext
							())?
					"double" : "long double")
			    << "*)&FPConstant" << I->second << ')';
		} else {
			double V;
			if (FPC->getType() ==
				Type::getFloatTy(CPV->getContext()))
				V = FPC->getValueAPF().
					convertToFloat();
			else if (FPC->getType() ==
				Type::getDoubleTy(CPV->
						getContext()))
				V = FPC->getValueAPF().
					convertToDouble();
			else {
				// Long double.  Convert the number to double, discarding precision.
				// This is not awesome, but it at least makes the CBE output somewhat
				// useful.
				APFloat Tmp = FPC->getValueAPF();
				bool LosesInfo;
				Tmp.convert(APFloat::IEEEdouble,
					APFloat::rmTowardZero,
					&LosesInfo);
				V = Tmp.convertToDouble();
			}

			if (IsNAN(V)) {
				// The value is NaN

				// FIXME the actual NaN bits should be emitted.
				// The prefix for a quiet NaN is 0x7FF8. For a signalling NaN,
				// it's 0x7ff4.
				const unsigned long QuietNaN =
					0x7ff8UL;
				//const unsigned long SignalNaN = 0x7ff4UL;

				// We need to grab the first part of the FP #
				char Buffer[100];

				uint64_t ll = DoubleToBits(V);
				sprintf(Buffer, "0x%llx", static_cast <long long >(ll));

				std::string Num(&Buffer[0], &Buffer[6]);
				unsigned long Val =
					strtoul(Num.c_str(), 0, 16);

				if (FPC->getType() ==
					Type::getFloatTy(FPC->
							getContext()))
					Out << "LLVM_NAN" << (Val
							==
							QuietNaN
							? ""
							:
							"S")
					    << "F(\"" << Buffer <<
						"\") /*nan*/ ";
				else
					Out << "LLVM_NAN" << (Val
							==
							QuietNaN
							? ""
							:
							"S")
					    << "(\"" << Buffer <<
						"\") /*nan*/ ";
			} else if (IsInf(V)) {
				// The value is Inf
				if (V < 0)
					Out << '-';
				Out << "LLVM_INF" <<
					(FPC->getType() ==
						Type::getFloatTy(FPC->
								getContext
								())? "F" :
						"")
				    << " /*inf*/ ";
			} else {
				std::string Num;
#if HAVE_PRINTF_A && ENABLE_CBE_PRINTF_A
				// Print out the constant as a floating point number.
				char Buffer[100];
				sprintf(Buffer, "%a", V);
				Num = Buffer;
#else
				SmallString<10> smallstr;
				FPC->getValueAPF().toString(smallstr, 5, 0);
				Num = smallstr.str().str();
#endif
				Out << Num;
			}
		}
		break;
	}

	case Type::ArrayTyID:
		// Use C99 compound expression literal initializer syntax.
		if (!Static) {
			Out << "(";
			printType(Out, CPV->getType());
			Out << ")";
		}
		Out << "{ ";	// Arrays are wrapped in struct types.
		if (ConstantArray * CA = dyn_cast < ConstantArray > (CPV)) {
			printConstantArray(CA, Static);
		} else {
			assert(isa < ConstantAggregateZero > (CPV) || isa < UndefValue > (CPV));
			const ArrayType *AT = cast < ArrayType > (CPV->getType());
			Out << '{';
			if (AT->getNumElements()) {
				Out << ' ';
				Constant *CZ =
					Constant::getNullValue(AT->
							getElementType
							());
				printConstant(CZ, Static);
				for (unsigned i = 1, e =
					     AT->getNumElements(); i != e; ++i) {
					Out << ", ";
					printConstant(CZ, Static);
				}
			}
			Out << " }";
		}
		Out << " }";	// Arrays are wrapped in struct types.
		break;

	case Type::VectorTyID:
		// Use C99 compound expression literal initializer syntax.
		if (!Static) {
			Out << "(";
			printType(Out, CPV->getType());
			Out << ")";
		}
		if (ConstantVector * CV =
			dyn_cast < ConstantVector > (CPV)) {
			printConstantVector(CV, Static);
		} else {
			assert(isa < ConstantAggregateZero > (CPV)
				|| isa < UndefValue > (CPV));
			const VectorType *VT =
				cast < VectorType > (CPV->getType());
			Out << "{ ";
			Constant *CZ =
				Constant::getNullValue(VT->getElementType());
			printConstant(CZ, Static);
			for (unsigned i = 1, e = VT->getNumElements();
			     i != e; ++i) {
				Out << ", ";
				printConstant(CZ, Static);
			}
			Out << " }";
		}
		break;

	case Type::StructTyID:
		// Use C99 compound expression literal initializer syntax.
		if (!Static) {
			Out << "(";
			printType(Out, CPV->getType());
			Out << ")";
		}
		if (isa < ConstantAggregateZero > (CPV)
			|| isa < UndefValue > (CPV)) {
			StructType *ST =
				cast < StructType > (CPV->getType());
			Out << '{';
			if (ST->getNumElements()) {
				Out << ' ';
				printConstant(Constant::
					getNullValue(ST->
						getElementType
						(0)), Static);
				for (unsigned i = 1, e =
					     ST->getNumElements(); i != e; ++i) {
					Out << ", ";
					printConstant(Constant::
						getNullValue(ST->
							getElementType
							(i)),
						Static);
				}
			}
			Out << " }";
		} else {
			Out << '{';
			if (CPV->getNumOperands()) {
				Out << ' ';
				printConstant(cast < Constant >
					(CPV->getOperand(0)),
					Static);
				for (unsigned i = 1, e =
					     CPV->getNumOperands(); i != e; ++i) {
					Out << ", ";
					printConstant(cast < Constant >
						(CPV->getOperand(i)),
						Static);
				}
			}
			Out << " }";
		}
		break;

	case Type::PointerTyID:
		if (isa < ConstantPointerNull > (CPV)) {
			Out << "(";
			printType(Out, CPV->getType());	// sign doesn't matter
			Out << "/*NULL*/0)";
			break;
		} else if (GlobalValue * GV =
			dyn_cast < GlobalValue > (CPV)) {
			writeOperand(GV, Static);
			break;
		}
		// FALL THROUGH
	default:
#ifndef NDEBUG
		errs() << "Unknown constant type: " << *CPV << "\n";
#endif
		llvm_unreachable(0);
	}
}



std::string PromelaWriter::GetValueName(const Value * Operand)
{
	std::string res;
	SmallString<10> smallstr;

	if (isa<Function>(Operand))

	// Mangle globals with the standard mangler interface for LLC compatibility.

	if (const GlobalValue * GV = dyn_cast < GlobalValue > (Operand)) {
		Mang->getNameWithPrefix(smallstr, GV, false);
		res = smallstr.str().str(); // -> StringRef -> String
		return replaceAll(res, ".", "_");
	}
	std::string Name = Operand->getName();

	if (Name.empty()) {	// Assign unique names to local temporaries.
		unsigned &No = AnonValueNumbers[Operand];
		if (No == 0)
			No = ++NextAnonValueNumber;
		Name = "tmp__" + utostr(No);
	}

	std::string VarName;
	VarName.reserve(Name.capacity());

	for (std::string::iterator I = Name.begin(), E = Name.end();
	     I != E; ++I) {
		char ch = *I;

		if (! ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_')) {
			char buffer[5];
			sprintf(buffer, "_%x_", ch);
			VarName += buffer;
		} else
			VarName += ch;
	}

	res = "llvm_cbe" + VarName;
	return replaceAll(res, ".", "_");
}

/// writeInstComputationInline - Emit the computation for the specified
/// instruction inline, with no destination provided.
void PromelaWriter::writeInstComputationInline(Instruction & I)
{
	// We can't currently support integer types other than 1, 8, 16, 32, 64.
	// Validate this.
	Type *Ty = I.getType();
	if (Ty->isIntegerTy() && (Ty != Type::getInt1Ty(I.getContext()) &&
					Ty != Type::getInt8Ty(I.getContext()) &&
					Ty != Type::getInt16Ty(I.getContext()) &&
					Ty != Type::getInt32Ty(I.getContext()) &&
					Ty != Type::getInt64Ty(I.getContext()))) {
		report_fatal_error("The Simple backend does not currently support integer types"
				"of widths other than 1, 8, 16, 32, 64.\n"
				"This is being tracked as PR 4158.");
	}	
		
	/*TRACE_6("***************************************************************");
	Ty->dump();
	TRACE_6 ("**************************************************************");
	if (isSystemCStruct(dyn_cast<StructType>(Ty)))
	      {
		//Out<<"INSIDE SYSTEMC VISITING----> \n";
		visitSystemCStruct(I);
		//return;
	      }*/
	// If this is a non-trivial bool computation, make sure to truncate down to
	// a 1 bit value.  This is important because we want "add i1 x, y" to return
	// "0" when x and y are true, not "2" for example.
	//bool NeedBoolTrunc = false;
	//if (I.getType() == Type::getInt1Ty(I.getContext()) && !isa < ICmpInst > (I) && !isa < FCmpInst > (I))
		//NeedBoolTrunc = true;

//	if (NeedBoolTrunc)
//		Out << "((";

	TRACE_4("/***** Visiting " << I.getOpcodeName() << " ( writeInstComputationInline() ) ******/ \n");
	//Out << "/***** Visiting " << I.getOpcodeName() << " ( writeInstComputationInline() ) ******/ \n";
	visit(I);
	//Out << "\n/***** Visited " << I.getOpcodeName() << "******/ \n";
	TRACE_4("\n");
	TRACE_4("/***** Visited " << I.getOpcodeName() << "******/ \n");

//	if (NeedBoolTrunc)
//		Out << ")&1)";
}

/// isAddressExposed - Return true if the specified value's name needs to
/// have its address taken in order to get a  value of the correct type.
/// This happens for global variables, byval parameters, and direct allocas.
bool PromelaWriter::isAddressExposed(const Value * V) const
{
	if (const Argument * A = dyn_cast < Argument > (V))
		return ByValParams.count(A);
	return isa < GlobalVariable > (V) || isDirectAlloca(V);
}


/// Deref - Print the result of dereferencing the specified
/// operand with '*'.  This is equivalent to printing '*' then using
/// writeOperand, but avoids excess syntax in some cases.
void PromelaWriter::writeOperandDeref(Value * Operand)
{
	if (isAddressExposed(Operand)) {
		// Already something with an address exposed.
		writeOperandInternal(Operand);
	} else {
		Out << "*(";
		writeOperand(Operand);
		Out << ")";
	}
}


void PromelaWriter::writeOperandInternal(Value * Operand, bool Static)
{
	if (Instruction * I = dyn_cast < Instruction > (Operand))
		// Should we inline this instruction to build a tree?
		if (isInlinableInst(*I) && !isDirectAlloca(I)) {
			//Out << '(';
			writeInstComputationInline(*I);
			return;
		}

	Constant *CPV = dyn_cast < Constant > (Operand);

	if (CPV && !isa < GlobalValue > (CPV))
		printConstant(CPV, Static);
	else
		Out << GetValueName(Operand);
}

void PromelaWriter::writeOperand(Value * Operand, bool Static)
{
	TRACE_4("/**** writeOperand() ****/\n");

//   bool isAddressImplicit = isAddressExposed(Operand);
//   if (isAddressImplicit)
//     Out << "(&";  // Global variables are referenced as their addresses by llvm

	writeOperandInternal(Operand, Static);

//   if (isAddressImplicit)
//     Out << ')';
}

/// FindStaticTors - Given a static ctor/dtor list, unpack its contents into
/// the StaticTors set.
static void FindStaticTors(GlobalVariable * GV,
			std::set < Function * >&StaticTors)
{
	ConstantArray *InitList =
		dyn_cast < ConstantArray > (GV->getInitializer());
	if (!InitList)
		return;

	for (unsigned i = 0, e = InitList->getNumOperands(); i != e; ++i)
		if (ConstantStruct * CS =
			dyn_cast < ConstantStruct >
			(InitList->getOperand(i))) {
			if (CS->getNumOperands() != 2)
				return;	// Not array of 2-element structs.

			if (CS->getOperand(1)->isNullValue())
				return;	// Found a null terminator, exit printing.
			Constant *FP = CS->getOperand(1);
			if (ConstantExpr * CE =
				dyn_cast < ConstantExpr > (FP))
				if (CE->isCast())
					FP = CE->getOperand(0);
			if (Function * F = dyn_cast < Function > (FP))
				StaticTors.insert(F);
		}
}

/// getGlobalVariableClass - If this is a global that is specially recognized
/// by LLVM, return a code that indicates how we should handle it.
static SpecialGlobalClass getGlobalVariableClass(const GlobalVariable * GV)
{
	// If this is a global ctors/dtors list, handle it now.
	if (GV->hasAppendingLinkage() && GV->use_empty()) {
		if (GV->getName() == "llvm.global_ctors")
			return GlobalCtors;
		else if (GV->getName() == "llvm.global_dtors")
			return GlobalDtors;
	}
	// Otherwise, it it is other metadata, don't print it.  This catches things
	// like debug information.
	if (GV->getSection() == "llvm.metadata")
		return NotPrinted;

	return NotSpecial;
}

// PrintEscapedString - Print each character of the specified string, escaping
// it if it is not printable or if it is an escape char.
static void PrintEscapedString(const char *Str, unsigned Length,
			raw_ostream & Out)
{
	for (unsigned i = 0; i != Length; ++i) {
		unsigned char C = Str[i];
		if (isprint(C) && C != '\\' && C != '"')
			Out << C;
		else if (C == '\\')
			Out << "\\\\";
		else if (C == '\"')
			Out << "\\\"";
		else if (C == '\t')
			Out << "\\t";
		else
			Out << "\\x" << hexdigit(C >> 4) << hexdigit(C &
								0x0F);
	}
}

// PrintEscapedString - Print each character of the specified string, escaping
// it if it is not printable or if it is an escape char.
static void PrintEscapedString(const std::string & Str, raw_ostream & Out)
{
	PrintEscapedString(Str.c_str(), Str.size(), Out);
}


/// Output all floating point constants that cannot be printed accurately...
void PromelaWriter::printFloatingPointConstants(Function & F)
{
	// Scan the module for floating point constants.  If any FP constant is used
	// in the function, we want to redirect it here so that we do not depend on
	// the precision of the printed form, unless the printed form preserves
	// precision.
	//
	for (constant_iterator I = constant_begin(&F), E =
		     constant_end(&F); I != E; ++I)
		printFloatingPointConstants(*I);

	Out << '\n';
}

void PromelaWriter::printFloatingPointConstants(const Constant * C)
{
	// If this is a constant expression, recursively check for constant fp values.
	if (const ConstantExpr * CE = dyn_cast < ConstantExpr > (C)) {
		for (unsigned i = 0, e = CE->getNumOperands(); i != e; ++i)
			printFloatingPointConstants(CE->getOperand(i));
		return;
	}
	// Otherwise, check for a FP constant that we need to print.
	const ConstantFP *FPC = dyn_cast < ConstantFP > (C);
	if (FPC == 0 ||
		// Do not put in FPConstantMap if safe.
		isFPCSafeToPrint(FPC) ||
		// Already printed this constant?
		FPConstantMap.count(FPC))
		return;

	FPConstantMap[FPC] = FPCounter;	// Number the FP constants

	if (FPC->getType() == Type::getDoubleTy(FPC->getContext())) {
		double Val = FPC->getValueAPF().convertToDouble();
		uint64_t i =
			FPC->getValueAPF().bitcastToAPInt().getZExtValue();
		Out << "static const ConstantDoubleTy FPConstant" <<
			FPCounter++ << " = 0x" << utohexstr(i)
		    << "ULL;    /* " << Val << " */\n";
	} else if (FPC->getType() == Type::getFloatTy(FPC->getContext())) {
		float Val = FPC->getValueAPF().convertToFloat();
		uint32_t i =
			(uint32_t) FPC->getValueAPF().
			bitcastToAPInt().getZExtValue();
		Out << "static const ConstantFloatTy FPConstant" <<
			FPCounter++ << " = 0x" << utohexstr(i)
		    << "U;    /* " << Val << " */\n";
	} else if (FPC->getType() ==
		Type::getX86_FP80Ty(FPC->getContext())) {
		// api needed to prevent premature destruction
		APInt api = FPC->getValueAPF().bitcastToAPInt();
		const uint64_t *p = api.getRawData();
		Out << "static const ConstantFP80Ty FPConstant" <<
			FPCounter++ << " = { 0x" << utohexstr(p[0])
		    << "ULL, 0x" << utohexstr((uint16_t) p[1]) <<
			",{0,0,0}" << "}; /* Long double constant */\n";
	} else if (FPC->getType() ==
		Type::getPPC_FP128Ty(FPC->getContext())
		|| FPC->getType() ==
		Type::getFP128Ty(FPC->getContext())) {
		APInt api = FPC->getValueAPF().bitcastToAPInt();
		const uint64_t *p = api.getRawData();
		Out << "static const ConstantFP128Ty FPConstant" <<
			FPCounter++ << " = { 0x" << utohexstr(p[0]) << ", 0x"
		    << utohexstr(p[1])
		    << "}; /* Long double constant */\n";

	} else {
		llvm_unreachable("Unknown float type!");
	}
}


void
PromelaWriter::insertTypeName(Type* Ty, std::string TyName)
{
	TRACE_4(" Type inserted: " << Ty << "   " << replaceAll(TyName, ".", "_") << "\n");
	this->TypeNames[Ty] = replaceAll(TyName, ".", "_");
}

int
PromelaWriter::getTypeNamesSize()
{
	return this->TypeNames.size();
}

/// printSymbolTable - Run through symbol table looking for type names.  If a
/// type name is found, emit its declaration...
///
void PromelaWriter::printModuleTypes(Module & M )
{
	TRACE_2("PromelaWriter > Emitting types\n");
	Out << "\n\n/*---- Types ----*/\n";

	TypeFinder TST;
	TST.run(M, true);// only named types will be collected.

	// We are only interested in the type plane of the symbol table.
	TypeFinder::const_iterator I = TST.begin();
	TypeFinder::const_iterator End = TST.end();

	// If there are no type names, exit early.
	if (I == End)
		return;

	// Print out forward declarations for structure types before anything else!
	Out << "/* Structure forward decls */\n";
	for (; I != End; ++I) {
		std::string Name = (*I)->getStructName().str();
		insertTypeName(*I, Name);
	}
	Out << '\n';

	std::vector < Process * >::iterator processIt = this->elab->getProcesses()->begin();
	std::vector < Process * >::iterator endIt = this->elab->getProcesses()->end();

	for (; processIt < endIt; ++processIt) {

		Process *proc = *processIt;
		Function::arg_iterator argI = proc->getMainFct()->arg_begin();
		const Value* moduleArg = &*argI;		
		PointerType* PTy = cast<PointerType>(moduleArg->getType());
		fillContainedStructs(PTy->getElementType(), true);
	}

	std::set < Type *>::iterator itS;
	for (itS = this->StructPrinted.begin() ; itS != this->StructPrinted.end(); ++itS) {
		// Print structure type out.
		Type* Ty = *itS;
		std::string Name = TypeNames[Ty];
		TRACE_5("Emitting type " << Name << "  " << Ty<< "\n");
		Out << "typedef ";
		printType(Out, Ty, true, Name, true);
		Out << ";\n\n";
	}


// 		std::vector < Function * >*usedFcts = proc->getUsedFunctions();
// 		currentProcess = proc;

// 		for (std::vector < Function * >::iterator itF = usedFcts->begin();
// 		     itF < usedFcts->end(); ++itF) {

// 			Function *F = *itF;
// 			TRACE_4("PromelaWriter > printing function : " << F->getNameStr() << "\n");

	
	// Now we can print out typedefs.  Above, we guaranteed that this can only be
	// for struct or opaque types.
// 	Out << "/* Typedefs */\n";
// 	for (I = TST.begin(); I != End; ++I) {
// 		std::string Name = "l_" + Mang->makeNameProper(I->first);
// 		Out << "typedef ";
// 		printType(Out, I->second, false, Name);
// 		Out << ";\n";
// 	}

// 	Out << '\n';

// 	// Keep track of which structures have been printed so far...
// 	std::set < Type *>StructPrinted;

// 	// Loop over all structures then push them into the stack so they are
// 	// printed in the correct order.
// 	//
// 	Out << "/* Structure contents */\n";
// 	for (I = TST.begin(); I != End; ++I)
// 		if (isa < StructType > (I->second)
// 			|| isa < ArrayType > (I->second))
// 			// Only print out used types!
// 			printContainedStructs(I->second, StructPrinted);
}

// Push the struct onto the stack and recursively push all structs
// this one depends on.
//
// TODO:  Make this work properly with vector types
//
bool PromelaWriter::fillContainedStructs(Type * Ty, bool fill)
{
	bool isEmpty = true;
	std::string Name = TypeNames[Ty];
//	TRACE_5("FillContainedStruct:" << Name << "\n");
	// Don't walk through pointers.
	if (isa < PointerType > (Ty) || Ty->isPrimitiveType() || Ty->isIntegerTy()) {
//		TRACE_5(" -> return false\n");
		return false;
	}
	
	// Print all contained types first.
	for (Type::subtype_iterator I = Ty->subtype_begin(), E = Ty->subtype_end(); I != E; ++I) {
		Type* subTy = *I;
		std::string SubName = TypeNames[subTy];
//		TRACE_5("Consider subtype: " << SubName << "\n");
		if (isSystemCType(subTy)) {
//			TRACE_5(" -> continue\n");
			continue;
		}
		
		if (! fillContainedStructs(subTy, fill))
			isEmpty = false;
	}
	if (isa < StructType > (Ty) || isa < ArrayType > (Ty)) {
//		TRACE_5("isa struct or isa array:" << Name << "\n");
		if (fill && ! isEmpty) {
//			TRACE_5("insert:" << Name << "\n");
			this->StructPrinted.insert(Ty);
		}
	}

	return isEmpty;
}

void PromelaWriter::printFunctionSignature(const Function * F,
					bool Prototype, bool inlineFct)
{
	TRACE_5("PromelaWriter > printing function signature\n");

	/// isStructReturn - Should this function actually return a struct by-value?
// 	bool isStructReturn = F->hasStructRetAttr();
	
	//   if (F->hasLocalLinkage()) Out << "static ";
	//   if (F->hasDLLImportLinkage()) Out << "__declspec(dllimport) ";
	//   if (F->hasDLLExportLinkage()) Out << "__declspec(dllexport) ";  
	switch (F->getCallingConv()) {
	case CallingConv::X86_StdCall:
		triggerError(Out, "NYI : callingconv (stcall here)");
		Out << "__attribute__((stdcall)) ";
		break;
	case CallingConv::X86_FastCall:
		triggerError(Out, "NYI : callingconv (fastcall here)");
		Out << "__attribute__((fastcall)) ";
		break;
	default:
		break;
	}
	
	// Loop over the arguments, printing them...
//	const AttrListPtr &PAL = F->getAttributes();
	FunctionType *FT = cast<FunctionType>(F->getFunctionType());
	
	if (FT->isVarArg()) {
		triggerError(Out, "Error : not able to manage vararg functions\n");
	}  
				
	// Print out the name...
	if (inlineFct)
		Out << "inline ";
	else
		Out << "proctype ";

	
	std::string fctName = GetValueName(F) + "_pnumber_" + intToString(currentProcess->getPid());
	Out << fctName << '(';
	
//	std::vector<pair<std::string, Type*> >* args = new std::vector<pair<std::string, Type*> >();
//	std::vector<pair<std::string, Type*> >* ret = new std::vector<pair<std::string, Type*> >();

//	fillDependencies(F, string(""), args, ret);
	
	bool printedArg = false;
//	if (!args->empty()) {
	Function::const_arg_iterator argI = F->arg_begin(), argE = F->arg_end();
//	++argI;
	for (; argI != argE; ++argI) {
		const Value* currentArg = &*argI;
		if (! isTypeEmpty(currentArg->getType())) { 
			std::string ArgName = GetValueName(currentArg);
			TRACE_7("PRINTING ARG : " << ArgName << "\n");
			if (printedArg)
				Out << ", ";
			if (inlineFct)
				Out << ArgName;
			else
				printType(Out, currentArg->getType(), /*isSigned*/true, ArgName, false);
			printedArg = true;
		}
	}
//	}
	Out << ')';
}

void
PromelaWriter::addVectors(std::vector<std::pair<std::string, Type*> >* from,
			std::vector<pair<std::string, Type*> >* to)
{
	std::vector<pair<std::string, Type*> >::iterator itFrom;
	for (itFrom = from->begin(); itFrom != from->end(); ++itFrom) {
		to->push_back(*itFrom);
	}
}

bool
PromelaWriter::isTypeEmpty(Type* ty)
{
	while (isa<PointerType>(ty)) {
 		ty = cast<PointerType>(ty)->getElementType();
	}

//	return Ty->getTypeID() == Type::StructTyID
	return fillContainedStructs(ty, false);
}

bool
PromelaWriter::isSystemCStruct(StructType* ty)
{	
	/*TRACE_6("--------******************************************************************************----\n");
	((Type*)ty)->dump();
	TRACE_6("--------******************************************************************************----\n");*/
	std::map < Type *, std::string >::iterator ITN = TypeNames.find(ty), ETN = TypeNames.end();
	if (ITN!=ETN){
 	//std::string typeName = this->TypeNames.find(ty)->second;
	std::string typeName = ITN->second;
	//Out<<"------------------------> isSystemCStruct > " << typeName << "\n";
 	return typeName.substr(0,10).compare("struct_sc_") == 0 || typeName.substr(0, 12).compare("struct_std::") == 0;
	}

	return false;
}

bool
PromelaWriter::isSystemCType(Type* ty)
{
 	while (isa<PointerType>(ty)) {
 		ty = cast<PointerType>(ty)->getElementType();
 	}

	if (ty->isPrimitiveType() || ty->isIntegerTy()) {
		return false;
	}

	if (this->TypeNames.find(ty) == this->TypeNames.end())
		return false;
 	std::string typeName =this->TypeNames.find(ty)->second;
	
	// clang deals systemc namespace with class_sc_core::
	// but llvm-gcc-4.2(llvm-2.8) uses struct_sc_core::
	bool res = (typeName.substr(0, 16).compare("struct_sc_core::") == 0 )
		|| (typeName.substr(0, 15).compare("class_sc_core::") == 0)
		|| (typeName.substr(0, 12).compare("struct_std::") == 0);
 	return res;

}


static inline bool isFPIntBitCast(const Instruction & I)
{
	if (!isa < BitCastInst > (I))
		return false;
	Type *SrcTy = I.getOperand(0)->getType();
	Type *DstTy = I.getType();
	return (SrcTy->isFloatingPointTy() && DstTy->isIntegerTy()) ||
		(DstTy->isFloatingPointTy() && SrcTy->isIntegerTy());
}

void PromelaWriter::printFunction(Function & F, bool inlineFct)
{
	/// isStructReturn - Should this function actually return a struct by-value?
// 	bool isStructReturn = F.hasStructRetAttr();

	printFunctionSignature(&F, false, inlineFct);
	Out << "\n";
	Out << "{\n";

	if (! inlineFct)
		Out << "atomic {\n";


	// If this is a struct return function, handle the result with magic.
// 	if (isStructReturn) {
// 		triggerError(Out, "NYI : function returning a struct.");
// 		Type *StructTy =
// 			cast < PointerType >
// 			(F.arg_begin()->getType())->getElementType();
// 		Out << "  ";
// 		printType(Out, StructTy, false, "StructReturn");
// 		Out << ";  /* Struct return temporary */\n";

// 		Out << "  ";
// 		printType(Out, F.arg_begin()->getType(), false,
// 			GetValueName(F.arg_begin()));
// 		Out << " = &StructReturn;\n";
// 	}

	TRACE_5("PromelaWriter > printing locals\n");

	// print local variable information for the function
	for (inst_iterator I = inst_begin(&F), E = inst_end(&F); I != E; ++I) {
		TRACE_7("LETS SEE IF THIS WORKS                   : " << GetValueName(&*I) << "  ->  " );
			I->dump();
		if (const AllocaInst * AI = isDirectAlloca(&*I)) {
			Out << "/* local variable */";
			printType(Out, AI->getAllocatedType(), false, GetValueName(AI));
			Out << ";\n    ";
			TRACE_7("Inside First If:-> Adding new local variable : " << GetValueName(&*I) << "  ->  " );
			I->dump();
			
		} else if (I->getType() != Type::getVoidTy(F.getContext()) && !isInlinableInst(*I) && ! isSystemCType(I->getType())) {
 			TRACE_7("Adding new local variable : " << GetValueName(&*I) << "  ->  " );
			I->dump();

			Out << "/* local variable */";
			printType(Out, I->getType(), false, GetValueName(&*I));
			Out << ";\n    ";

			if (isa < PHINode > (*I)) {	// Print out PHI node temporaries as well...
				printType(Out, I->getType(), false, GetValueName(&*I) +	"__PHI_TEMPORARY");
				Out << ";\n    ";
			}
		} else {
			Out << "/* Not printing " << GetValueName(&*I) << "*/\n";
		}
		// We need a temporary for the BitCast to use so it can pluck a value out
		// of a union to do the BitCast. This is separate from the need for a
		// variable to hold the result of the BitCast. 
		if (isFPIntBitCast(*I)) {
			ErrorMsg << "NYI : bitcast ????";
			triggerError(Out);
			Out << "    llvmBitCastUnion " << GetValueName(&*I)
			    << "__BITCAST_TEMPORARY;\n";
		}
	}
	Out << "\n";
	TRACE_5("PromelaWriter > printing basic blocks\n");

	// print the basic blocks
	for (Function::iterator BB = F.begin(), E = F.end(); BB != E; ++BB) {
			printBasicBlock(BB);
	}

	if (currentProcess->getPid() == 0 &&  this->insertBug) {
		Out << "    assert(false);\n";
	} else {
		Out << "finished[" << currentProcess->getPid() << "] = true;\n";
	}

	if (! inlineFct)
		Out << "}\n";
       
	Out << "}\n\n";
}

void PromelaWriter::printBasicBlock(BasicBlock * BB)
{
	TRACE_6("PromelaWriter > printing basic block : " << BB->getName().str() << "\n");

	// Don't print the label for the basic block if there are no uses, or if
	// the only terminator use is the predecessor basic block's terminator.
	// We have to scan the use list because PHI nodes use basic blocks too but
	// do not require a label to be generated.
	//
	bool NeedsLabel = false;
	for (pred_iterator PI = pred_begin(BB), E = pred_end(BB); PI != E; ++PI) {
		NeedsLabel = true;
		break;
	}
	
	if (NeedsLabel)
		Out << "\n" << GetValueName(BB) << ":\n";
	Out << "    skip;\n";

	// Output all of the instructions in the basic block...
	for (BasicBlock::iterator II = BB->begin(), E = --BB->end();
	     II != E; ++II) {
		if (!isInlinableInst(*II) && !isDirectAlloca(II) && !isSystemCType(II->getType())) {
			if (II->getType() != Type::getVoidTy(BB->getContext()) && !isInlineAsm(*II)) {
	TRACE_6("/**** before outputLValue ****/");
				outputLValue(II);

			} else {
				Out << "    ";
			}
			pointerToInst = &*II;
			writeInstComputationInline(*II);
			Out << ";\n";
		} else {
			TRACE_4("/***** SKIPPING inlinable inst or direct alloca ****/\n");
		}
	}

	TRACE_5("/***** Visit terminator : " << *BB->getTerminator()->getOpcodeName() << "*****/");
	// Don't emit prefix or suffix for the terminator.
	visit(*BB->getTerminator());
	TRACE_5 ("/***** Visited terminator ****/");

}


// Specific Instruction type classes... note that all of the casts are
// necessary because we use the instruction classes as opaque types...
//

void PromelaWriter::visitReturnInst(ReturnInst & I)
{
	TRACE_4("/***** Visiting return inst ****/\n");

	// If this is a struct return function, return the temporary struct.
// 	bool isStructReturn =
// 		I.getParent()->getParent()->hasStructRetAttr();

// 	if (isStructReturn) {
// 		Out << "  return StructReturn;\n";
// 		return;
// 	}
	// Don't output a void return if this is the last basic block in the function
	if (I.getNumOperands() == 0 &&
		&*--I.getParent()->getParent()->end() == I.getParent() &&
		!I.getParent()->size() == 1) {
		return;
	}

	TRACE_4("/**** Nb operands : " << I.
		getNumOperands() << "****/\n");

	if (I.getNumOperands() >= 1) {
//     Out << "  {\n";
//     Out << "    ";
//     printType(Out, I.getParent()->getParent()->getReturnType());
		Out << "   llvm_cbe_mrv_temp = ";
		for (unsigned i = 0, e = I.getNumOperands(); i != e; ++i) {
			Out << "    ";
			writeOperand(I.getOperand(i));
			if (i != e - 1)
				Out << ",";
			Out << "\n";
		}
		Out << "    ;\n";
		Out << "    return llvm_cbe_mrv_temp;\n";
		return;
	}
//   Out << ";\n";
}
void PromelaWriter::visitSystemCStruct(Instruction &I)
{
	Out<<"INSIDE SYSTEMC VISITING----> \n";
        Type* Ty=I.getType();
	std::string typeName = this->TypeNames.find(Ty)->second;
	if (typeName.find("struct_sc_uint")!=string::npos){
		Out<<I.getOperand(0);
		Out<<";\n";
	}
}
	
void PromelaWriter::visitSwitchInst(SwitchInst & SI)
{
	Out << "    switch (";
	writeOperand(SI.getOperand(0));
	Out << ") {\n  default:\n";
	printPHICopiesForSuccessor(SI.getParent(), SI.getDefaultDest(), 2);
	printBranchToBlock(SI.getParent(), SI.getDefaultDest(), 2);
	Out << ";\n";
	for (unsigned i = 2, e = SI.getNumOperands(); i != e; i += 2) {
		Out << "    case ";
		writeOperand(SI.getOperand(i));
		Out << ":\n";
		BasicBlock *Succ =
			cast < BasicBlock > (SI.getOperand(i + 1));
		printPHICopiesForSuccessor(SI.getParent(), Succ, 2);
		printBranchToBlock(SI.getParent(), Succ, 2);
		if (Function::iterator(Succ) ==
			next(Function::iterator(SI.getParent())))
			Out << "    break;\n";
	}
	Out << "    }\n";
}

void PromelaWriter::visitInvokeInst(InvokeInst & I)
{
	llvm_unreachable("Lowerinvoke pass didn't work!");
}

//void PromelaWriter::visitUnwindInst(UnwindInst & I)
//{
	//llvm_unreachable("Lowerinvoke pass didn't work!");
//}

void PromelaWriter::visitUnreachableInst(UnreachableInst & I)
{
	Out << "    /*UNREACHABLE*/;\n";
}

void PromelaWriter::visitInstruction(Instruction & I)
{
#ifndef NDEBUG
	errs() << "C Writer does not know about " << I;
#endif
	llvm_unreachable(0);
}

void PromelaWriter::outputLValue(Instruction * I)
{
	Out << "    /*OutputLValue*/ " << GetValueName(I) << " = ";
}

bool PromelaWriter::printPHICopiesForSuccessor(BasicBlock * CurBlock,
					BasicBlock * Successor,
					unsigned Indent)
{
	bool printed = false;
	for (BasicBlock::iterator I = Successor->begin(); isa < PHINode > (I); ++I) {
		PHINode *PN = cast < PHINode > (I);
		// Now we have to do the printing.
		Value *IV = PN->getIncomingValueForBlock(CurBlock);
		if (!isa < UndefValue > (IV)) {
			Out << std::string(Indent, ' ');
			Out << "    " << GetValueName(I) << "__PHI_TEMPORARY = ";
			writeOperand(IV);
			Out << "    /* for PHI node */";
			printed = true;
		}
	}
	return printed;
}

void PromelaWriter::printBranchToBlock(BasicBlock * CurBB,
				BasicBlock * Succ, unsigned Indent)
{
	Out << std::string(Indent, ' ') << "    goto ";
	writeOperand(Succ);
	Out << ";";
}

/***************************************************************************
 **************** Visit functions ******************************************
 **************************************************************************/


// Branch instruction printing - Avoid printing out a branch to a basic block
// that immediately succeeds the current one.
//
void PromelaWriter::visitBranchInst(BranchInst & I)
{
	if (I.isConditional()) {
		Out << "    if\n";
		Out << "        :: ";
		writeOperand(I.getCondition());
		Out << " -> "; 
		
		if (printPHICopiesForSuccessor(I.getParent(), I.getSuccessor(0), 2))
			Out << ";        \n";
		printBranchToBlock(I.getParent(), I.getSuccessor(0), 2);
		
		Out << "\n        :: else -> ";
		
		if (printPHICopiesForSuccessor(I.getParent(), I.getSuccessor(1), 2))
			Out << ";        \n";
		printBranchToBlock(I.getParent(), I.getSuccessor(1), 2);
		
		Out << "\n    fi;\n";
	} else {
		if (printPHICopiesForSuccessor(I.getParent(), I.getSuccessor(0), 0))
			Out << ";    \n";
		printBranchToBlock(I.getParent(), I.getSuccessor(0), 0);
	}
	Out << "\n";
}

// PHI nodes get copied into temporary values at the end of predecessor basic
// blocks.  We now need to copy these temporary values into the REAL value for
// the PHI.
void PromelaWriter::visitPHINode(PHINode & I)
{
	writeOperand(&I);
	Out << "__PHI_TEMPORARY";
}


void PromelaWriter::visitBinaryOperator(Instruction & I)
{
	// binary instructions, shift instructions, setCond instructions.
	assert(!isa < PointerType > (I.getType()));
	TRACE_4("/**** visitBinaryOperator() ****/\n");

	// We must cast the results of binary operations which might be promoted.
//   bool needsCast = false;
//   if ((I.getType() == Type::getInt8Ty(I.getContext())) ||
//       (I.getType() == Type::getInt16Ty(I.getContext())) 
//       || (I.getType() == Type::getFloatTy(I.getContext()))) {
//     needsCast = true;
//     Out << "((";
//     printType(Out, I.getType(), false);
//     Out << ")(";
//   }

	// If this is a negation operation, print it out as such.  For FP, we don't
	// want to print "-0.0 - X".
	if (BinaryOperator::isNeg(&I)) {
		Out << "-(";
		writeOperand(BinaryOperator::
			getNegArgument(cast < BinaryOperator > (&I)));
		Out << ")";
	} else if (BinaryOperator::isFNeg(&I)) {
		Out << "-(";
		writeOperand(BinaryOperator::
			getFNegArgument(cast < BinaryOperator >
					(&I)));
		Out << ")";
	} else if (I.getOpcode() == Instruction::FRem) {
		// Output a call to fmod/fmodf instead of emitting a%b
		if (I.getType() == Type::getFloatTy(I.getContext()))
			Out << "fmodf(";
		else if (I.getType() == Type::getDoubleTy(I.getContext()))
			Out << "fmod(";
		else		// all 3 flavors of long double
			Out << "fmodl(";
		writeOperand(I.getOperand(0));
		Out << ", ";
		writeOperand(I.getOperand(1));
		Out << ")";
	} else {

		// Write out the cast of the instruction's value back to the proper type
		// if necessary.
//     bool NeedsClosingParens = writeInstructionCast(I);

		// Certain instructions require the operand to be forced to a specific type
		// so we use writeOperandWithCast here instead of writeOperand. Similarly
		// below for operand 1
		//    writeOperandWithCast(I.getOperand(0), I.getOpcode());
		writeOperand(I.getOperand(0), I.getOpcode());

		switch (I.getOpcode()) {
		case Instruction::Add:
		case Instruction::FAdd:
			Out << " + ";
			break;
		case Instruction::Sub:
		case Instruction::FSub:
			Out << " - ";
			break;
		case Instruction::Mul:
		case Instruction::FMul:
			Out << " * ";
			break;
		case Instruction::URem:
		case Instruction::SRem:
		case Instruction::FRem:
			Out << " % ";
			break;
		case Instruction::UDiv:
		case Instruction::SDiv:
		case Instruction::FDiv:
			Out << " / ";
			break;
		case Instruction::And:
			Out << " & ";
			break;
		case Instruction::Or:
			Out << " | ";
			break;
		case Instruction::Xor:
			Out << " ^ ";
			break;
		case Instruction::Shl:
			Out << " << ";
			break;
		case Instruction::LShr:
		case Instruction::AShr:
			Out << " >> ";
			break;
		default:
#ifndef NDEBUG
			errs() << "Invalid operator type!" << I;
#endif
			llvm_unreachable(0);
		}

		//    writeOperandWithCast(I.getOperand(1), I.getOpcode());
		writeOperand(I.getOperand(1), I.getOpcode());
//     if (NeedsClosingParens)
//       Out << "))";
	}

//   if (needsCast) {
//     Out << "))";
//   }
}

void PromelaWriter::visitICmpInst(ICmpInst & I)
{
	// We must cast the results of icmp which might be promoted.
//   bool needsCast = false;

	// Write out the cast of the instruction's value back to the proper type
	// if necessary.
//   bool NeedsClosingParens = writeInstructionCast(I);

	// Certain icmp predicate require the operand to be forced to a specific type
	// so we use writeOperandWithCast here instead of writeOperand. Similarly
	// below for operand 1
//   writeOperandWithCast(I.getOperand(0), I);
	writeOperand(I.getOperand(0));

	switch (I.getPredicate()) {
	case ICmpInst::ICMP_EQ:
		Out << " == ";
		break;
	case ICmpInst::ICMP_NE:
		Out << " != ";
		break;
	case ICmpInst::ICMP_ULE:
	case ICmpInst::ICMP_SLE:
		Out << " <= ";
		break;
	case ICmpInst::ICMP_UGE:
	case ICmpInst::ICMP_SGE:
		Out << " >= ";
		break;
	case ICmpInst::ICMP_ULT:
	case ICmpInst::ICMP_SLT:
		Out << " < ";
		break;
	case ICmpInst::ICMP_UGT:
	case ICmpInst::ICMP_SGT:
		Out << " > ";
		break;
	default:
#ifndef NDEBUG
		errs() << "Invalid icmp predicate!" << I;
#endif
		llvm_unreachable(0);
	}

//   writeOperandWithCast(I.getOperand(1), I);
	writeOperand(I.getOperand(1));
//   if (NeedsClosingParens)
//     Out << "))";

//   if (needsCast) {
//     Out << "))";
//   }
}

void PromelaWriter::visitFCmpInst(FCmpInst & I)
{
	if (I.getPredicate() == FCmpInst::FCMP_FALSE) {
		Out << "0";
		return;
	}
	if (I.getPredicate() == FCmpInst::FCMP_TRUE) {
		Out << "1";
		return;
	}

	const char *op = 0;
	switch (I.getPredicate()) {
	default:
		llvm_unreachable("Illegal FCmp predicate");
	case FCmpInst::FCMP_ORD:
		op = "ord";
		break;
	case FCmpInst::FCMP_UNO:
		op = "uno";
		break;
	case FCmpInst::FCMP_UEQ:
		op = "ueq";
		break;
	case FCmpInst::FCMP_UNE:
		op = "une";
		break;
	case FCmpInst::FCMP_ULT:
		op = "ult";
		break;
	case FCmpInst::FCMP_ULE:
		op = "ule";
		break;
	case FCmpInst::FCMP_UGT:
		op = "ugt";
		break;
	case FCmpInst::FCMP_UGE:
		op = "uge";
		break;
	case FCmpInst::FCMP_OEQ:
		op = "oeq";
		break;
	case FCmpInst::FCMP_ONE:
		op = "one";
		break;
	case FCmpInst::FCMP_OLT:
		op = "olt";
		break;
	case FCmpInst::FCMP_OLE:
		op = "ole";
		break;
	case FCmpInst::FCMP_OGT:
		op = "ogt";
		break;
	case FCmpInst::FCMP_OGE:
		op = "oge";
		break;
	}

	Out << "llvm_fcmp_" << op << "(";
	// Write the first operand
	writeOperand(I.getOperand(0));
	Out << ", ";
	// Write the second operand
	writeOperand(I.getOperand(1));
	Out << ")";
}

static const char *getFloatBitCastField(Type * Ty)
{
	switch (Ty->getTypeID()) {
	default:
		llvm_unreachable("Invalid Type");
	case Type::FloatTyID:
		return "Float";
	case Type::DoubleTyID:
		return "Double";
	case Type::IntegerTyID:{
		unsigned NumBits =
			cast < IntegerType > (Ty)->getBitWidth();
		if (NumBits <= 32)
			return "Int32";
		else
			return "Int64";
	}
	}
}

void PromelaWriter::visitCastInst(CastInst & I)
{
	Type *DstTy = I.getType();
	Type *SrcTy = I.getOperand(0)->getType();

	if (isFPIntBitCast(I)) {
		Out << '(';
		// These int<->float and long<->double casts need to be handled specially
		Out << GetValueName(&I) << "__BITCAST_TEMPORARY."
		    << getFloatBitCastField(I.getOperand(0)->
					    getType()) << " = ";
		writeOperand(I.getOperand(0));
		Out << ", " << GetValueName(&I) << "__BITCAST_TEMPORARY."
		    << getFloatBitCastField(I.getType());
		Out << ')';
		return;
	}
//   Out << '(';
//   printCast(I.getOpcode(), SrcTy, DstTy);

	// Make a sext from i1 work by subtracting the i1 from 0 (an int).
	if (SrcTy == Type::getInt1Ty(I.getContext()) &&
		I.getOpcode() == Instruction::SExt)
		Out << "0-";

	writeOperand(I.getOperand(0));

	if (DstTy == Type::getInt1Ty(I.getContext()) &&
		(I.getOpcode() == Instruction::Trunc ||
			I.getOpcode() == Instruction::FPToUI ||
			I.getOpcode() == Instruction::FPToSI ||
			I.getOpcode() == Instruction::PtrToInt)) {
		// Make sure we really get a trunc to bool by anding the operand with 1 
		Out << "&1u";
	}
//   Out << ')';
}

void PromelaWriter::visitSelectInst(SelectInst & I)
{
	/*ErrorMsg << "NYI : SelectInst\n";
	triggerError(Out);*/

	Out << "((";
	writeOperand(I.getCondition());
	Out << ") -> (";
	writeOperand(I.getTrueValue());
	Out << ") : (";
	writeOperand(I.getFalseValue());
	Out << "))";
}


void PromelaWriter::lowerIntrinsics(Function & F)
{
	// This is used to keep track of intrinsics that get generated to a lowered
	// function. We must generate the prototypes before the function body which
	// will only be expanded on first use (by the loop below).
	std::vector < Function * >prototypesToGen;

	// Examine all the instructions in this function to find the intrinsics that
	// need to be lowered.
	for (Function::iterator BB = F.begin(), EE = F.end(); BB != EE;
	     ++BB)
		for (BasicBlock::iterator I = BB->begin(), E = BB->end();
		     I != E;)
			if (CallInst * CI = dyn_cast < CallInst > (I++))
				if (Function * F = CI->getCalledFunction())
					switch (F->getIntrinsicID()) {
					case Intrinsic::not_intrinsic:
						// Since LLVM3.2 have developed
						// atomic instruction. Atomic
						// not available as llvm
						// instrinsic anymore.
						// refer to visitFenceInst()
					//case Intrinsic::memory_barrier:
					case Intrinsic::vastart:
					case Intrinsic::vacopy:
					case Intrinsic::vaend:
					case Intrinsic::returnaddress:
					case Intrinsic::frameaddress:
					case Intrinsic::setjmp:
					case Intrinsic::longjmp:
					case Intrinsic::prefetch:
						// MM: don't know what
						//this was, but it
						//doesn't exist anymore.
						//case Intrinsic::dbg_stoppoint:
					case Intrinsic::powi:
					case Intrinsic::x86_sse_cmp_ss:
					case Intrinsic::x86_sse_cmp_ps:
					case Intrinsic::x86_sse2_cmp_sd:
					case Intrinsic::x86_sse2_cmp_pd:
					case Intrinsic::ppc_altivec_lvsl:
						// We directly implement these intrinsics
						break;
					default:
						// If this is an intrinsic that directly corresponds to a GCC
						// builtin, we handle it.
						const char *BuiltinName =
							"";
#define GET_GCC_BUILTIN_NAME
#include "llvm/Intrinsics.gen"
#undef GET_GCC_BUILTIN_NAME
						// If we handle it, don't lower it.
						if (BuiltinName[0])
							break;

						// All other intrinsic calls we must lower.
						Instruction *Before = 0;
						if (CI != &BB->front())
							Before =
								prior
								(BasicBlock::
									iterator(CI));

						IL->LowerIntrinsicCall(CI);
						if (Before) {	// Move iterator to instruction after call
							I = Before;
							++I;
						} else {
							I = BB->begin();
						}
						// If the intrinsic got lowered to another call, and that call has
						// a definition then we need to make sure its prototype is emitted
						// before any calls to it.
						if (CallInst * Call =
							dyn_cast < CallInst >
							(I))
							if (Function *
								NewF =
								Call->
								getCalledFunction
								())
								if (!NewF->
									isDeclaration
									())
									prototypesToGen.
										push_back
										(NewF);

						break;
					}
	// We may have collected some prototypes to emit in the loop above. 
	// Emit them now, before the function that uses them is emitted. But,
	// be careful not to emit them twice.
	std::vector < Function * >::iterator I = prototypesToGen.begin();
	std::vector < Function * >::iterator E = prototypesToGen.end();
	for (; I != E; ++I) {
		if (intrinsicPrototypesAlreadyGenerated.insert(*I).second) {
			Out << '\n';
			printFunctionSignature(*I, true, true);
			Out << ";\n";
		}
	}
}

void
PromelaWriter::printCodingGlobals()
{
 	int nbThreads = this->elab->getNumProcesses();
 	int nbEvents = this->elab->getNumEvents();
//	int ct;
	
	Out << 	"#define NBTHREADS " << nbThreads << "\n"
		"#define NBEVENTS " << nbEvents << "\n"
		"bool finished[NBTHREADS];\n\n"
//		"int Tg;\n"
		"int T[NBTHREADS];\n"
//		"int S[NBEVENTS];\n"
		"\n";
	
	if (this->eventsAsBool) {
		Out <<	"bool waiters[NBTHREADS];\n\n";
			
		vector < Process * >::iterator processIt = this->elab->getProcesses()->begin();
		vector < Process * >::iterator endIt = this->elab->getProcesses()->end();

		for (; processIt < endIt; ++processIt) {
			Process *proc = *processIt;
			std::vector < Event* >* events = proc->getEvents();
			vector < Event * >::iterator evIt = events->begin();
			vector < Event * >::iterator endEvIt = events->end();
		
			for (; evIt < endEvIt; ++evIt) {
				Event* event = *evIt;
				Out << "bool " << getEventName(proc, event) << " = false;\n"; 
			}
		}
	} else {
		
		Out <<	"int e[NBTHREADS];\n";
	}
	Out << "\n";
}

void
PromelaWriter::printWaitTimePrimitive()
{
	int nbThreads = this->elab->getNumProcesses();
	int ct;

	if (this->relativeClocks) {
		triggerError(Out, "NYI : relative encoding for time in PromelaBackend\n\n");
	}

	Out << ""
		"inline wait(pnumber, time)\n"
		"{\n"
		//		"    assert(T[pnumber] == Tg);\n"
		"    T[pnumber] = T[pnumber] + time;         \n";
	
	if (this->eventsAsBool) {
		for (ct = 0; ct < nbThreads - 1; ct++) {
			Out << "( finished[" << intToString(ct) << "] || waiters[" << intToString(ct) << "] || T[pnumber] <= T[" << intToString(ct) << "] ) &&\n";
		}
		Out << "( finished[" << intToString(nbThreads - 1) << "] || waiters[" << intToString(nbThreads - 1) << "] || T[pnumber] <= T[" << intToString(nbThreads - 1) << "] ) &&\n";

	} else {

		for (ct = 0; ct < nbThreads - 1; ct++) {
			Out << "    ( finished[" + intToString(ct) + "] || e[" + intToString(ct) + "] != 0 || T[pnumber] <= T["+ intToString(ct) + "] ) && \n";
		}
		Out << "    ( finished[" + intToString(nbThreads - 1) + "] || e[" + intToString(nbThreads - 1) + "] != 0 || T[pnumber] <= T["+ intToString(nbThreads - 1) + "] );\n";
//	Out <<"    Tg = T[pnumber];\n"
	}

	Out << "}\n\n";

}


void
PromelaWriter::printNotifyPrimitive()
{
	
	if (this->eventsAsBool) {
		
		vector < Event * >::iterator evIt = this->elab->getEvents()->begin();
		vector < Event * >::iterator endEvIt = this->elab->getEvents()->end();
		
		for (; evIt < evIt; ++evIt) {
			
			Event* event = *evIt;
			Out << "inline notify_" << getEventName(NULL, event) << " (pnumber)\n"
				"{\n";
			
			vector < Process * >::iterator processIt = event->getProcesses()->begin();
			vector < Process * >::iterator endIt = event->getProcesses()->end();
			
			for (; processIt < endIt; ++processIt) {
				Process *proc = *processIt;
				
				Out << "    if\n";
				Out << "      :: " << getEventName(proc, event) << " == true ->\n";
//					Out << "        assert(T[" << proc->getPid() << "] <= T[pnumber]);\n";
				Out << "        " << getEventName(proc, event) << " = false;\n";
				Out << "        T[" << proc->getPid() << "] = T[pnumber];\n";
				Out << "        waiters[" << proc->getPid() << "] = false;\n";
				Out << "      :: (" << getEventName(proc, event) << ") == false -> skip;\n";
				Out << "    fi;\n";
			}
			Out << "}\n\n";
		}
	} else {
		vector < Event * >::iterator evIt = this->elab->getEvents()->begin();
		vector < Event * >::iterator endEvIt = this->elab->getEvents()->end();
		
		for (; evIt < endEvIt; ++evIt) {
			
			Event* event = *evIt;
			Out << "inline notify_" << getEventName(NULL, event) << " (pnumber)\n"
				"{\n";
			
			vector < Process * >::iterator processIt = event->getProcesses()->begin();
			vector < Process * >::iterator endIt = event->getProcesses()->end();
			
			for (; processIt < endIt; ++processIt) {
				Process *proc = *processIt;
				
				Out << "    if\n"
					"    :: e[" << proc->getPid() << "] == " << event->getNumEvent() << "  ->\n"
					"       e[" << proc->getPid() << "] = 0;\n"
//						"       assert(T[" << proc->getPid() << "] <= T[pnumber]);\n"
					"       T[" << proc->getPid() << "]=T[pnumber];\n"
					"    :: else -> skip;"
					"\n";
				Out << "    fi;\n";
			}
			Out << "}\n\n";					
		}
	}
//			Out << "    S[" << event->getNumEvent() << "] = " << "T[pnumber];\n";
//		}
//	}
}

void
PromelaWriter::printWaitEventPrimitive()
{
// 	int nbThreads = this->elab->getNumProcesses();
// 	int ct;

	vector < Process * >::iterator processIt = this->elab->getProcesses()->begin();
	vector < Process * >::iterator endIt = this->elab->getProcesses()->end();
	
	for (; processIt < endIt; ++processIt) {
		Process *proc = *processIt;
		std::vector < Event* >* events = proc->getEvents();
		vector < Event * >::iterator evIt = events->begin();
		vector < Event * >::iterator endEvIt = events->end();
		
		for (; evIt < endEvIt; ++evIt) {
			Event* event = *evIt;
			
			Out << "inline wait_" << getEventName(proc, event) << "()\n"
				"{\n";
			
			if (this->eventsAsBool) {
				Out <<  ""
//					"    assert(T[" << proc->getPid() << "] == Tg);\n"
					"    " << getEventName(proc, event) << " = true;\n"
					"    waiters[" << proc->getPid() << "] = true;\n"
					"     " << getEventName(proc, event) << " == false;\n";
//					"    assert(T[" << proc->getPid() << "] == S[" << event->getNumEvent() << "]);\n"
//					"    Tg = T[" << proc->getPid() << "];\n";
			} else {
				Out <<  ""
//					"    assert(T[" << proc->getPid() << "] == Tg);\n"
					"    e[" << proc->getPid() << "] = " << event->getNumEvent() << ";\n"
					"    e[" << proc->getPid() << "] == 0;\n";
//					"    assert(T[" << proc->getPid() << "] == S[" << event->getNumEvent() << "]);\n"
//					"    Tg = T[" << proc->getPid() << "];\n";
			}
			Out << "}\n\n";
		}
	}
}



void
PromelaWriter::printPrimitives()
{
	Out << "\n\n/*---- Encoding ----*/\n";
	printCodingGlobals();
	printWaitTimePrimitive();
	printNotifyPrimitive();
	printWaitEventPrimitive();
}
bool notPrinted(string ObjName){
    	static vector<string> declaredObjs;
	static int size;
	declaredObjs.resize(++size);
	vector<string>::iterator st=declaredObjs.begin();
	vector<string>::iterator end=declaredObjs.end();
	for (;st!=end;st++)
		if (!ObjName.compare(*st))
			return false;
	declaredObjs.push_back(ObjName);
	return true;
}
bool notInitialized(string ObjName){
    	static vector<string> declaredObjs;
	static int size;
	declaredObjs.resize(++size);
	vector<string>::iterator st=declaredObjs.begin();
	vector<string>::iterator end=declaredObjs.end();
	for (;st!=end;st++)
		if (!ObjName.compare(*st))
			return false;
	declaredObjs.push_back(ObjName);
	return true;
}

void
PromelaWriter::printRandFct()
{
	Out << ""
		"      inline randnr()"
		"      {	/*"
		"      	 * don't call this rand()..."
		"      	 * to avoid a clash with the C library routine"
		"      	 */"
		"      	do"
		"      	:: nr++		/* randomly increment */"
		"      	:: nr--		/* or decrement       */"
		"      	:: break	/* or stop            */"
		"      	do;"
		"      }";
}

void
PromelaWriter::printGlobalVariables(Mangler* mang)
{
	vector < GlobalValue * >::iterator globalIt = this->elab->getGlobalVariables()->begin();
	vector < GlobalValue * >::iterator globalEnd = this->elab->getGlobalVariables()->end();
	vector <Channel *>::iterator ChannelIt=this->elab->getChannels()->begin();
	vector <Channel *>::iterator ChannelEnd=this->elab->getChannels()->end();
	TRACE_2("PromelaWriter > Emitting Global variables\n");

	Out << "/*---- Global variables ----*/\n";
	for (; globalIt < globalEnd; ++globalIt) {
		GlobalValue* gv = *globalIt;
		SmallString<10> smallstr;

		mang->getNameWithPrefix(smallstr, gv, false);
		std::string name = "llvm_cbe" + smallstr.str().str();
		printType(Out, gv->getType(), false, name);
		Out << ";\n";
	}
	Out << "/*---- Channel variables ----*/\n";
	for (; ChannelIt < ChannelEnd; ++ChannelIt) {
		Channel* chan = *ChannelIt;
		
		if (notPrinted(chan->getTypeName())){
		string name = "llvm_chan_" + chan->getTypeName();
		printType(Out, chan->getType(), false, name);
		Out << ";\n";
		}
	}
}

void
PromelaWriter::printProcesses()
{
	vector < Process * >::iterator processIt = this->elab->getProcesses()->begin();
	vector < Process * >::iterator endIt = this->elab->getProcesses()->end();

	TRACE_2("PromelaWriter > Emitting Processes\n");
	Out << "\n\n/*---- Threads ----*/\n";
	for (; processIt < endIt; ++processIt) {

		Process *proc = *processIt;
		std::vector < Function * >*usedFcts = proc->getUsedFunctions();
		currentProcess = proc;
		this->scjit->setCurrentProcess(currentProcess);
		TRACE_3("PromelaWriter > printing process : " << proc->getName() << "\n");
		TRACE_3("Info : nb of used functions : " << usedFcts->size() << "\n");

		for (std::vector < Function * >::iterator itF = usedFcts->begin();
		     itF < usedFcts->end(); ++itF) {

			Function *F = *itF;
			currentFunction = F;
			TRACE_4("PromelaWriter > printing function : " << F->getName().str() << "\n");
			
			// Do not codegen any 'available_externally' functions at all, they have
			// definitions outside the translation unit.
			if (F->hasAvailableExternallyLinkage())
				continue;

			// Get rid of intrinsics we can't handle.
			lowerIntrinsics(*F);

			// Output all floating point constants that cannot be printed accurately.
			printFloatingPointConstants(*F);

			printFunction(*F, false);
		}
	}
}
bool ifNotDeclared(string ObjName)
{
	static std::vector<std::string> declaredObjs;
	static int size;
	declaredObjs.resize(++size);
	std::vector<std::string>::iterator st=declaredObjs.begin();
	std::vector<std::string>::iterator end=declaredObjs.end();
	for (;st!=end;st++)
		if (!ObjName.compare(*st))
			return false;
	declaredObjs.push_back(ObjName);
	return true;

}
	
 

void
PromelaWriter::printInitSection()
{
	std::vector < Process * >::iterator processIt = this->elab->getProcesses()->begin();
	std::vector < Process * >::iterator endIt = this->elab->getProcesses()->end();
	std::vector < GlobalValue * >::iterator globalIt = this->elab->getGlobalVariables()->begin();
	std::vector < GlobalValue * >::iterator globalEnd = this->elab->getGlobalVariables()->end();
	std::vector <Channel *>::iterator ChannelIt=this->elab->getChannels()->begin();
	std::vector <Channel *>::iterator ChannelEnd=this->elab->getChannels()->end();
	
	TRACE_2("PromelaWriter > Emitting init section \n");
	Out << "/* ----------------------------- Init --------------------------------- */\n"
		"init\n{\n"
		"    int i = 0;\n";

	if (this->eventsAsBool) {
		Out << ""
//			"    Tg=0;\n"
			"    do\n"
			"      :: i == NBTHREADS -> break;\n"
			"      :: else ->\n"
			"         T[i] = 0;\n"
			"         waiters[i]= false;\n"
			"         finished[i] = false;\n"
			"         i++;\n"
			"    od;\n"
// 			"    i = 0;\n"
// 			"    do\n"
// 			"      :: i == NBEVENTS -> break;\n"
// 			"      :: else ->\n"
// 			"         S[i] = 0;\n"
// 			"         i++;\n"
// 			"    od;\n"
			"    \n";
	} else {
		Out <<  ""
//			"    Tg=0;\n"
			"    do\n"
			"      :: i == NBTHREADS -> break;\n"
			"      :: else ->\n"
			"         e[i] = 0;\n"
			"         T[i] = 0;\n"
			"         finished[i] = false;\n"
			"         i++;\n"
 			"    od;\n"
// 			"    i = 0;\n"
// 			"    do\n"
// 			"      :: i == NBEVENTS -> break;\n"
// 			"      :: else ->\n"
// 			"         S[i] = 0;\n"
// 			"         i++;\n"
// 			"    od;\n"
			"\n";
	}

	for (; processIt < endIt; ++processIt) {
		Process *proc = *processIt;
		if (!isTypeEmpty(proc->getMainFct()->arg_begin()->getType())) {
			//const std::string fctName = GetValueName(proc->getMainFct()) + "_pnumber_" + intToString(proc->getPid());
			if (ifNotDeclared(proc->getModule()->getUniqueName()))
  			{
				const std::string modName = "mod_" + proc->getModule()->getUniqueName();
				Out << "    ";
				printType(Out, proc->getMainFct()->arg_begin()->getType() , false, modName, false, AttrListPtr());
				Out << ";\n";
			}
		}
	}
	for (; globalIt < globalEnd; ++globalIt) {
		GlobalValue* gv = *globalIt;

		SmallString<10> smallstr;
		Mang->getNameWithPrefix(smallstr, gv, false);
		std::string NameSoFar = "llvm_cbe" + smallstr.str().str();
		
		Type *Ty=gv->getType();
		while (isa<PointerType>(Ty)) {
			Ty = cast<PointerType>(Ty)->getElementType();
		}	

		switch (Ty->getTypeID()) {
			case Type::VoidTyID:
				Out << NameSoFar<<" = NULL";
				break;
			case Type::IntegerTyID:{
				unsigned NumBits = cast < IntegerType > (Ty)->getBitWidth();
				if (NumBits == 1)
					Out <<NameSoFar<<" = true";
				else if (NumBits <= 8)
					Out << NameSoFar<< " = 0";
				else if (NumBits <= 16)
					Out << NameSoFar<< " = 0";
				else
					Out << NameSoFar<< " = 0";
				}
				break;
			/*case Type::FloatTyID:
			triggerError(Out, "NYI : float type");
			case Type::DoubleTyID:
			triggerError(Out, "NYI : double type");
			case Type::X86_FP80TyID:
			case Type::PPC_FP128TyID:
			case Type::FP128TyID:
			triggerError(Out, "NYI : long type");
			case Type::VectorTyID:{
			triggerError(Out, "NYI : Vector type");
			const VectorType *VTy = cast < VectorType > (Ty);
			printInitialValue(Out, VTy->getElementType(),
				isSigned,
				NameSoFar + "[" + utostr(TD->getTypeAllocSize(VTy))+ "]");	
			}*/

			default:
			#ifndef NDEBUG
			errs() << "Unknown primitive type: " << *Ty << "\n";
			#endif
			llvm_unreachable(0);
		}
	//printInitialValue(Out, gv->getType(), false, name);
	Out << ";\n";
	}
	for (; ChannelIt < ChannelEnd; ++ChannelIt) {
		Channel* chan = *ChannelIt;
		std::string NameSoFar = "llvm_chan_" + chan->getTypeName();
		if (notInitialized(NameSoFar)){		
			Type *Ty=chan->getType();
			switch (Ty->getTypeID()) {
				case Type::VoidTyID:
					Out << NameSoFar<<" = NULL";
					break;
				case Type::IntegerTyID:{
					unsigned NumBits = cast < IntegerType > (Ty)->getBitWidth();
					if (NumBits == 1)
						Out << NameSoFar<<" = true";
					else if (NumBits <= 8)
						Out << NameSoFar<< " = 0";
					else if (NumBits <= 16)
						Out << NameSoFar<< " = 0";
					else
						Out << NameSoFar<< " = 0";
					}
					break;
				/*case Type::FloatTyID:
				triggerError(Out, "NYI : float type");
				case Type::DoubleTyID:
				triggerError(Out, "NYI : double type");
				case Type::X86_FP80TyID:
				case Type::PPC_FP128TyID:
				case Type::FP128TyID:
				triggerError(Out, "NYI : long type");
				case Type::VectorTyID:{
				triggerError(Out, "NYI : Vector type");
				const VectorType *VTy = cast < VectorType > (Ty);
				printInitialValue(Out, VTy->getElementType(),
				isSigned,
				NameSoFar + "[" + utostr(TD->getTypeAllocSize(VTy))+ "]");	
				}*/

				default:
				#ifndef NDEBUG
				errs() << "Unknown primitive type: " << *Ty << "\n";
				#endif
				llvm_unreachable(0);
			}
		Out << ";\n";
		}
	}
	

	Out << "\n";
	Out <<	"    atomic {\n";

	processIt = this->elab->getProcesses()->begin();
	for (; processIt < endIt; ++processIt) {
		Process *proc = *processIt;
		const std::string fctName = GetValueName(proc->getMainFct()) + "_pnumber_" + intToString(proc->getPid());
		if (! isTypeEmpty(proc->getMainFct()->arg_begin()->getType())) {
			const std::string modName = "mod_" + proc->getModule()->getUniqueName();
			Out << "        run " + fctName + "(" << modName << ");\n";
		} else {
			Out << "        run " + fctName + "();\n";
		}
	}
	
	Out << "    }\n}\n";
}

std::string
PromelaWriter::getEventName(Process* proc, Event* event)
{
	std::stringstream ss;
	if (proc == NULL)
		ss << event->getEventName();
	else
		ss << event->getEventName() << "_" << proc->getPid();
	return ss.str();
}

/*
 * TODO : manage the case where events and time couldn't be determined statically
 */
void
PromelaWriter::visitSCConstruct(SCConstruct * scc)
{
	TimeConstruct* tc;
	NotifyConstruct* notifyC;
	EventConstruct* eventC;	
	ReadConstruct* rc;
	WriteConstruct* wc;
	AssertConstruct *ac;
	Event* event;
	Port* port;
	std::vector<Channel*>* channels;
	std::vector<Channel*>::iterator channelsIt;
	SimpleChannel* sc;

	TRACE_4("/***** visitSCConstruct() *****/\n");
	switch(scc->getID()) {
	case WAITEVENTCONSTRUCT:
		eventC = (EventConstruct*) scc;
		event = eventC->getWaitedEvent();
		if (eventC->isStaticallyFound()) {
			Out << "wait_" << getEventName(currentProcess, event) << "();\n";
		} else {
			Out << "/* TODO: wait_e() on event not determined statically  */";
                        /* todo */
		}
		break;
	case NOTIFYCONSTRUCT:
		notifyC = (NotifyConstruct*) scc;
		if (notifyC->isStaticallyFound()) {
			event = notifyC->getNotifiedEvent();
			Out << "    notify_" << getEventName(NULL, event) << "(" << currentProcess->getPid() << ");\n";
		} else {
			Out << "/* TODO: notify() event not determined statically */";
			/* todo */
		}
		break;
	case TIMECONSTRUCT:
		tc = (TimeConstruct*) scc;
		if (tc->isStaticallyFound()) {
			Out << "wait(" << currentProcess->getPid() << ", ";
			Out << intToString(tc->getTime());
			Out << ")";
		} else {
			Out << "/* Todo: wait() time not determined statically */";
			/* todo */
		}
		break;
	case READCONSTRUCT:
		rc = (ReadConstruct*) scc;
		port = rc->getPort();
		TRACE_3("Visit READ\n");
		if (port->getChannelID() == SIMPLE_CHANNEL) {
			if (port->getChannels()->size() != 1 ) {
				ERROR("Reading in a port binded to more than one channel is not possible\n");
			} else {
				SimpleChannel* sc = (SimpleChannel*) port->getChannel();
				Out << "llvm_chan_"<<sc->getTypeName();
			}
		} else {
			Out << "/* TODO : read() on a non simple channel */";
			/* todo */
		}
		break;
	case WRITECONSTRUCT:
		wc = (WriteConstruct*) scc;		
		port = wc->getPort();
		channels = port->getChannels();
		TRACE_3("Visit WRITE\n");
		switch(port->getChannelID()) {
		case SIMPLE_CHANNEL:
			for (channelsIt = channels->begin() ; channelsIt != channels->end() ; ++channelsIt) {
				sc = (SimpleChannel*) *channelsIt;
//			if (port->getGlobalVariableType()->getTypeID() != Type::PointerTyID) {
				//Out << "/* write() on simpleport */\n";
				Out << "llvm_chan_"<<sc->getTypeName()<< " = " << wc->getValue();
 				if (channelsIt != channels->end())
 					Out << ";\n";
//			}
			break;
		case FORWARDING_CHANNEL:
			Out << "/* TODO : write() on a forwarding channel */";
			/* todo */
			for (channelsIt = channels->begin() ; channelsIt != channels->end() ; ++channelsIt) {
				/* NYI */
			}
			break;
		case FIFO_CHANNEL:
			Out << "/* TODO : write() on a FIFO */";
			/* todo */
			for (channelsIt = channels->begin() ; channelsIt != channels->end() ; ++channelsIt) {
				/* NYI */
			}
			break;
		case CLOCK_CHANNEL:
			/* IMPOSSIBLE case */
			ERROR("How can we write on a CLOCK channel ??\n");
			break;
		default:
			Out << "/* write() on port ??? */\n";
			break;
 		}

// 		if (wc->isStaticallyFound()) {
// 			Out << wc->getGlobalVariable() << " = " << wc->getValue() << "\n";
// 		} else {
			/* todo */
 		}
		break;
	case ASSERTCONSTRUCT:
		ac = (AssertConstruct*) scc;
		if (ac->isStaticallyFound()) {
			Out << "assert(" << ac->getCond() <<");";
		} else {
			Out << "assert(" << GetValueName(ac->getMissingCond()) << ");";
		}
		break;
	case RANDCONSTRUCT:
		Out << "randnr(nr);";
		break;
	default:
		ErrorMsg << "Construction not managed in Promela backend: " << scc->getID();
		triggerError(Out);
	}
}

void PromelaWriter::visitCallInst(CallInst & I)
{
	std::map <Instruction*, std::map<Process*, SCConstruct*> >::iterator itC;
	CallInst* pI = cast<CallInst>(pointerToInst);
	itC = this->sccfactory->getConstructs()->find(pI);
	if (itC != this->sccfactory->getConstructs()->end()) {
		std::map<Process*, SCConstruct * > CbyP = itC->second;
		return visitSCConstruct(CbyP.find(currentProcess)->second);
	}
	TRACE_4("Visiting CallInst, function is : " << I.getCalledFunction() << "\n");
//	TRACE_4("Visiting CallInst, function is : " << I.getCalledFunction()->getNameStr() << "\n");
	if (isa < InlineAsm > (I.getOperand(0)))
		return visitInlineAsm(I);

	bool WroteCallee = false;

	// Handle intrinsic function calls first...
	if (Function * F = I.getCalledFunction())
		if (Intrinsic::ID ID = (Intrinsic::ID) F->getIntrinsicID())
			if (visitBuiltinCall(I, ID, WroteCallee))
				return;


	Value *Callee = I.getCalledValue();

	PointerType *PTy = cast < PointerType > (Callee->getType());
	FunctionType *FTy = cast < FunctionType > (PTy->getElementType());

	TRACE_4("Visiting CallInst, function TYPE IS : " << PTy << " and " << FTy << "\n");

	if (! I.getCalledFunction()) {
		Value* arg = I.getOperand(1);
		Function * cf = currentFunction;
		void* fctAddr = this->scjit->jitAddr(cf, &I, arg);
		const GlobalValue* gv = this->scjit->getEngine()->getGlobalValueAtAddress(fctAddr);
		TRACE_4("################# Function jitted : " << fctAddr << "\n");
		TRACE_4("################# GlobalValue corresponding : " << gv << "\n");
		if (! gv)
			return;
		else
			Callee = (Function*) gv;
	}


	// If this is a call to a struct-return function, assign to the first
	// parameter instead of passing it to the call.
	const AttrListPtr & PAL = I.getAttributes();
// 	bool hasByVal = I.hasByValArgument();
//  	bool isStructRet = I.hasStructRetAttr();
// 	if (isStructRet) {
// 		writeOperandDeref(I.getOperand(1));
// 		Out << " = ";
// 	}

	if (I.isTailCall())
		Out << " /*tail*/ ";

	if (!WroteCallee) {
		// If this is an indirect call to a struct return function, we need to cast
		// the pointer. Ditto for indirect calls with byval arguments.

		// GCC is a real PITA.  It does not permit codegening casts of functions to
		// function pointers if they are in a call (it generates a trap instruction
		// instead!).  We work around this by inserting a cast to void* in between
		// the function and the function pointer cast.  Unfortunately, we can't just
		// form the constant expression here, because the folder will immediately
		// nuke it.
		//
		// Note finally, that this is completely unsafe.  ANSI C does not guarantee
		// that void* and function pointers have the same size. :( To deal with this
		// in the common case, we handle casts where the number of arguments passed
		// match exactly.
		//
		if (ConstantExpr * CE = dyn_cast < ConstantExpr > (Callee)) {
			if (CE->isCast())
				if (Function * RF = dyn_cast < Function > (CE->getOperand(0))) {
					Callee = RF;
				}
		}
		writeOperand(Callee);
		Out << "_pnumber_" << currentProcess->getPid();
		Function* cf = pI->getCalledFunction();
		Function* pf = pI->getParent()->getParent();
		if (cf->getArgumentList().size() != 0) {
			Value* arg = pI->getOperand(1);
			void* mod = this->scjit->jitAddr(pf, pI, arg);
			TRACE_4("################# Module jitted : " << mod << "\n");
			TRACE_4("################# IRModule associated : " << this->elab->getIRModule(mod) << "\n");
		}
	}

	Out << '(';

	unsigned NumDeclaredParams = FTy->getNumParams();

	CallSite::arg_iterator AI = I.op_begin() + 1, AE = I.op_end();
	unsigned ArgNo = 0;
// 	if (isStructRet) {	// Skip struct return argument.
// 		++AI;
// 		++ArgNo;
// 	}

	bool PrintedArg = false;
	for (; AI != AE; ++AI, ++ArgNo) {
		if (AI->getUser()->getNumUses() == 0) {
			continue;
		}
		if (PrintedArg)
			Out << ", ";
		if (ArgNo < NumDeclaredParams &&
			(*AI)->getType() != FTy->getParamType(ArgNo)) {
			Out << '(';
			printType(Out, FTy->getParamType(ArgNo),/*isSigned= */ PAL.paramHasAttr(ArgNo + 1, this->getAttributes(Attributes::SExt)));
			Out << ')';
		}
		// Check if the argument is expected to be passed by value.
		if (I.paramHasAttr(ArgNo + 1, Attributes::ByVal))
			writeOperandDeref(*AI);
		else
			writeOperand(*AI);
		PrintedArg = true;
	}
	Out << ')';
}

/// visitBuiltinCall - Handle the call to the specified builtin.  Returns true
/// if the entire call is handled, return false it it wasn't handled, and
/// optionally set 'WroteCallee' if the callee has already been printed out.
bool PromelaWriter::visitBuiltinCall(CallInst & I, Intrinsic::ID ID,
				bool & WroteCallee)
{
	switch (ID) {
	default:{
		// If this is an intrinsic that directly corresponds to a GCC
		// builtin, we emit it here.
		const char *BuiltinName = "";
		Function *F = I.getCalledFunction();
#define GET_GCC_BUILTIN_NAME
#include "llvm/Intrinsics.gen"
#undef GET_GCC_BUILTIN_NAME
		assert(BuiltinName[0]
			&& "Unknown LLVM intrinsic!");

		Out << BuiltinName;
		WroteCallee = true;
		return false;
	}

	case Intrinsic::vastart:
		Out << "0; ";

		Out << "va_start(*(va_list*)";
		writeOperand(I.getOperand(1));
		Out << ", ";
		// Output the last argument to the enclosing function.
		if (I.getParent()->getParent()->arg_empty()) {
			std::string msg;
			raw_string_ostream Msg(msg);
			Msg <<
				"The C backend does not currently supoprt zero "
			    << "argument varargs functions, such as '" <<
				I.getParent()->getParent()->getName() << "'!";
			report_fatal_error(Msg.str());
		}
		writeOperand(--I.getParent()->getParent()->arg_end());
		Out << ')';
		return true;
	case Intrinsic::vaend:
		if (!isa < ConstantPointerNull > (I.getOperand(1))) {
			Out << "0; va_end(*(va_list*)";
			writeOperand(I.getOperand(1));
			Out << ')';
		} else {
			Out << "va_end(*(va_list*)0)";
		}
		return true;
	case Intrinsic::vacopy:
		Out << "0; ";
		Out << "va_copy(*(va_list*)";
		writeOperand(I.getOperand(1));
		Out << ", *(va_list*)";
		writeOperand(I.getOperand(2));
		Out << ')';
		return true;
	case Intrinsic::returnaddress:
		Out << "__builtin_return_address(";
		writeOperand(I.getOperand(1));
		Out << ')';
		return true;
	case Intrinsic::frameaddress:
		Out << "__builtin_frame_address(";
		writeOperand(I.getOperand(1));
		Out << ')';
		return true;
	case Intrinsic::powi:
		Out << "__builtin_powi(";
		writeOperand(I.getOperand(1));
		Out << ", ";
		writeOperand(I.getOperand(2));
		Out << ')';
		return true;
	case Intrinsic::setjmp:
		Out << "setjmp(*(jmp_buf*)";
		writeOperand(I.getOperand(1));
		Out << ')';
		return true;
	case Intrinsic::longjmp:
		Out << "longjmp(*(jmp_buf*)";
		writeOperand(I.getOperand(1));
		Out << ", ";
		writeOperand(I.getOperand(2));
		Out << ')';
		return true;
	case Intrinsic::prefetch:
		Out << "LLVM_PREFETCH((const void *)";
		writeOperand(I.getOperand(1));
		Out << ", ";
		writeOperand(I.getOperand(2));
		Out << ", ";
		writeOperand(I.getOperand(3));
		Out << ")";
		return true;
	case Intrinsic::stacksave:
		// Emit this as: Val = 0; *((void**)&Val) = __builtin_stack_save()
		// to work around GCC bugs (see PR1809).
		Out << "0; *((void**)&" << GetValueName(&I)
		    << ") = __builtin_stack_save()";
		return true;
/*
MM: don't know what this was, but doesn't exist anymore.
	case Intrinsic::dbg_stoppoint:{
		// If we use writeOperand directly we get a "u" suffix which is rejected
		// by gcc.
		DbgStopPointInst & SPI =
			cast < DbgStopPointInst > (I);
		std::string dir;
		GetConstantStringInfo(SPI.getDirectory(), dir);
		std::string file;
		GetConstantStringInfo(SPI.getFileName(), file);
		Out << "\n#line " << SPI.getLine()
		    << " \"" << dir << '/' << file << "\"\n";
		return true;
	}
*/
	case Intrinsic::x86_sse_cmp_ss:
	case Intrinsic::x86_sse_cmp_ps:
	case Intrinsic::x86_sse2_cmp_sd:
	case Intrinsic::x86_sse2_cmp_pd:
		Out << '(';
		printType(Out, I.getType());
		Out << ')';
		// Multiple GCC builtins multiplex onto this intrinsic.
		switch (cast < ConstantInt >
			(I.getOperand(3))->getZExtValue()) {
		default:
			llvm_unreachable("Invalid llvm.x86.sse.cmp!");
		case 0:
			Out << "__builtin_ia32_cmpeq";
			break;
		case 1:
			Out << "__builtin_ia32_cmplt";
			break;
		case 2:
			Out << "__builtin_ia32_cmple";
			break;
		case 3:
			Out << "__builtin_ia32_cmpunord";
			break;
		case 4:
			Out << "__builtin_ia32_cmpneq";
			break;
		case 5:
			Out << "__builtin_ia32_cmpnlt";
			break;
		case 6:
			Out << "__builtin_ia32_cmpnle";
			break;
		case 7:
			Out << "__builtin_ia32_cmpord";
			break;
		}
		if (ID == Intrinsic::x86_sse_cmp_ps
			|| ID == Intrinsic::x86_sse2_cmp_pd)
			Out << 'p';
		else
			Out << 's';
		if (ID == Intrinsic::x86_sse_cmp_ss
			|| ID == Intrinsic::x86_sse_cmp_ps)
			Out << 's';
		else
			Out << 'd';

		Out << "(";
		writeOperand(I.getOperand(1));
		Out << ", ";
		writeOperand(I.getOperand(2));
		Out << ")";
		return true;
	case Intrinsic::ppc_altivec_lvsl:
		Out << '(';
		printType(Out, I.getType());
		Out << ')';
		Out << "__builtin_altivec_lvsl(0, (void*)";
		writeOperand(I.getOperand(1));
		Out << ")";
		return true;
	}
}

//TODO: assumptions about what consume arguments from the call are likely wrong
//      handle communitivity
void PromelaWriter::visitInlineAsm(CallInst & CI)
{
	triggerError(Out, "Cannot handle inline ASM\n");
}

void PromelaWriter::visitAllocaInst(AllocaInst & I)
{
	TRACE_4("visitAllocaInst()\n");
	Out << '(';
	printType(Out, I.getType());
	Out << ") alloca(sizeof(";
	printType(Out, I.getType()->getElementType());
	Out << ')';
	if (I.isArrayAllocation()) {
		Out << " * ";
		writeOperand(I.getOperand(0));
	}
	Out << ')';
}

void
PromelaWriter::printGEPExpression(Value * Ptr, gep_type_iterator I,	gep_type_iterator E, bool Static)
{
	TRACE_4("/**** printGEPExpression() ****/\n");

	// If there are no indices, just print out the pointer.
	if (I == E) {
		writeOperand(Ptr);
		return;
	}
	// Find out if the last index is into a vector.  If so, we have to print this
	// specially.  Since vectors can't have elements of indexable type, only the
	// last index could possibly be of a vector element.
	VectorType *LastIndexIsVector = 0;
	{
		for (gep_type_iterator TmpI = I; TmpI != E; ++TmpI)
			LastIndexIsVector =
				dyn_cast < VectorType > (*TmpI);
	}
	//Out << "(";

	// If the last index is into a vector, we can't print it as &a[i][j] because
	// we can't index into a vector with j in GCC.  Instead, emit this as
	// (((float*)&a[i])+j)
	if (LastIndexIsVector) {
		Out << "((";
		printType(Out,
			PointerType::getUnqual(LastIndexIsVector->
					getElementType()));
		Out << ")(";
	}
//   Out << '&';

	// If the first index is 0 (very typical) we can do a number of
	// simplifications to clean up the code.
	Value *FirstOp = I.getOperand();
	if (!isa < Constant > (FirstOp)
		|| !cast < Constant > (FirstOp)->isNullValue()) {
		TRACE_4("/****  first index is not 0 ****/\n");
		// First index isn't simple, print it the hard way.
		writeOperand(Ptr);
	} else {
		++I;		// Skip the zero index.

		TRACE_4("/****  first index is 0 ****/\n");

		// Okay, emit the first operand. If Ptr is something that is already address
		// exposed, like a global, avoid emitting (&foo)[0], just emit foo instead.
		if (isAddressExposed(Ptr)) {
			TRACE_4("   /**** address is exposed ****/\n");
			writeOperandInternal(Ptr, Static);
		} 
		   else	if (I != E && isa < StructType > (*I)) {
			TRACE_4("/**** struct type ****/\n");
			if (isSystemCStruct(dyn_cast<StructType>(*I)))
	      			{	
					StructType *ty=dyn_cast<StructType>(*I);	
					std::string typeName="";			
					std::map < Type *, std::string >::iterator ITN = TypeNames.find(ty), ETN = TypeNames.end();
					if (ITN!=ETN)
					typeName = ITN->second;
					//if (typeName.find("struct_sc")!=string::npos)
						//Out<<"TEST";
						writeOperand(Ptr); 			
							
					
	     			}
			// If we didn't already emit the first operand, see if we can print it as
			// P->f instead of "P[0].f"
			else{
			//Out<<"TESTERS";
			writeOperand(Ptr);
			Out << ".field" << cast < ConstantInt >
				(I.getOperand())->getZExtValue();
			}
//       Out << "->field" << cast<ConstantInt>(I.getOperand())->getZExtValue();
			++I;	// eat the struct index as well.
		} else {
			TRACE_4("/**** else case... ****/\n");

			// Instead of emitting P[0][1], emit (*P)[1], which is more idiomatic.
			Out << "(*";
			writeOperand(Ptr);
			Out << ")";
		}
	}

	for (; I != E; ++I) {
		if (isa < StructType > (*I)) {
		if (!isSystemCStruct(dyn_cast<StructType>(*I)))
			{
			//Out<<"Test";
			Out << ".field" << cast < ConstantInt >
				(I.getOperand())->getZExtValue();}
		} else if (isa < ArrayType > (*I)) {
			Out << ".array[";
//       writeOperandWithCast(I.getOperand(), Instruction::GetElementPtr);
			writeOperand(I.getOperand(),
				Instruction::GetElementPtr);
			Out << ']';
		} else if (!isa < VectorType > (*I)) {
			Out << '[';
//       writeOperandWithCast(I.getOperand(), Instruction::GetElementPtr);
			writeOperand(I.getOperand(),
				Instruction::GetElementPtr);
			Out << ']';
		} else {
			// If the last index is into a vector, then print it out as "+j)".  This
			// works with the 'LastIndexIsVector' code above.
			if (isa < Constant > (I.getOperand()) &&
				cast < Constant >
				(I.getOperand())->isNullValue()) {
				Out << "))";	// avoid "+0".
			} else {
				Out << ")+(";
//         writeOperandWithCast(I.getOperand(), Instruction::GetElementPtr);
				writeOperand(I.getOperand(),
					Instruction::GetElementPtr);
				Out << "))";
			}
		}
	}
	//Out << ")";
}

void PromelaWriter::writeMemoryAccess(Value * Operand,
				Type * OperandType,
				bool IsVolatile, unsigned Alignment)
{
//   bool IsUnaligned = Alignment &&
//     Alignment < TD->getABITypeAlignment(OperandType);

//   if (!IsUnaligned)
//     Out << '*';
//   if (IsVolatile || IsUnaligned) {
//     Out << "((";
//     if (IsUnaligned)
//       Out << "struct __attribute__ ((packed, aligned(" << Alignment << "))) {";
//     printType(Out, OperandType, false, IsUnaligned ? "data" : "volatile*");
//     if (IsUnaligned) {
//       Out << "; } ";
//       if (IsVolatile) Out << "volatile ";
//       Out << "*";
//     }
//     Out << ")";
//   }

	writeOperand(Operand);
//   if (IsVolatile || IsUnaligned) {
//     Out << ')';
//     if (IsUnaligned)
//       Out << "->data";
//   }
}

void PromelaWriter::visitLoadInst(LoadInst & I)
{
	writeMemoryAccess(I.getOperand(0), I.getType(), I.isVolatile(),
			I.getAlignment());

}

void PromelaWriter::visitStoreInst(StoreInst & I)
{
	writeMemoryAccess(I.getPointerOperand(),
			I.getOperand(0)->getType(), I.isVolatile(),
			I.getAlignment());
	Out << " = /* visitStore */";
	Value *Operand = I.getOperand(0);
	writeOperand(Operand);
}

void PromelaWriter::visitGetElementPtrInst(GetElementPtrInst & I)
{
	//Out << "VisitGetElementPtr \n";
	printGEPExpression(I.getPointerOperand(), gep_type_begin(I), gep_type_end(I), false);
}

void PromelaWriter::visitVAArgInst(VAArgInst & I)
{
	Out << "va_arg(*(va_list*)";
	writeOperand(I.getOperand(0));
	Out << ", ";
	printType(Out, I.getType());
	Out << ");\n ";
}

void PromelaWriter::visitInsertElementInst(InsertElementInst & I)
{
	Type *EltTy = I.getType()->getElementType();
	writeOperand(I.getOperand(0));
	//Out << ";\n  ";
	Out << "((";
	printType(Out, PointerType::getUnqual(EltTy));
	Out << ")(&" << GetValueName(&I) << "))[";
	writeOperand(I.getOperand(2));
	Out << "] = (";
	writeOperand(I.getOperand(1));
	Out << ")";
}

void PromelaWriter::visitExtractElementInst(ExtractElementInst & I)
{
	// We know that our operand is not inlined.
	Out << "((";
	Type *EltTy =
		cast < VectorType >
		(I.getOperand(0)->getType())->getElementType();
	printType(Out, PointerType::getUnqual(EltTy));
	Out << ")(&" << GetValueName(I.getOperand(0)) << "))[";
	writeOperand(I.getOperand(1));
	Out << "]";
}

void PromelaWriter::visitShuffleVectorInst(ShuffleVectorInst & SVI)
{
	Out << "(";
	printType(Out, SVI.getType());
	Out << "){ ";
	VectorType *VT = SVI.getType();
	unsigned NumElts = VT->getNumElements();
	Type *EltTy = VT->getElementType();

	for (unsigned i = 0; i != NumElts; ++i) {
		if (i)
			Out << ", ";
		int SrcVal = SVI.getMaskValue(i);
		if ((unsigned) SrcVal >= NumElts * 2) {
			Out << " 0/*undef*/ ";
		} else {
			Value *Op =
				SVI.getOperand((unsigned) SrcVal >= NumElts);
			if (isa < Instruction > (Op)) {
				// Do an extractelement of this value from the appropriate input.
				Out << "((";
				printType(Out,
					PointerType::getUnqual(EltTy));
				Out << ")(&" << GetValueName(Op)
				    << "))[" << (SrcVal & (NumElts - 1)) <<
					"]";
			} else if (isa < ConstantAggregateZero > (Op)
				|| isa < UndefValue > (Op)) {
				Out << "0";
			} else {
				printConstant(cast < ConstantVector >
					(Op)->
					getOperand(SrcVal &
						(NumElts - 1)),
					false);
			}
		}
	}
	Out << "}";
}

void PromelaWriter::visitInsertValueInst(InsertValueInst & IVI)
{
	// Start by copying the entire aggregate value into the result variable.
	writeOperand(IVI.getOperand(0));
	Out << ";\n  ";
	
	// Then do the insert to update the field.
	Out << GetValueName(&IVI);
	for (const unsigned *b = IVI.idx_begin(), *i = b, *e =
		     IVI.idx_end(); i != e; ++i) {
		Type *IndexedTy =
			ExtractValueInst::getIndexedType(IVI.getOperand(0)->
							getType(),ArrayRef<unsigned int>(b, i + 1) );
		if (isa < ArrayType > (IndexedTy))
			Out << ".array[" << *i << "]";
		else
			Out << ".field" << *i;
	}
	Out << " = /* insertvalue */";
	writeOperand(IVI.getOperand(1));
}

void PromelaWriter::visitExtractValueInst(ExtractValueInst & EVI)
{
	//Out << "(";
	if (isa < UndefValue > (EVI.getOperand(0))) {
		Out << "(";
		printType(Out, EVI.getType());
		Out << ") 0/*UNDEF*/";
	} else {
		Out << GetValueName(EVI.getOperand(0));
		for (const unsigned *b = EVI.idx_begin(), *i = b, *e =
			     EVI.idx_end(); i != e; ++i) {
			Type *IndexedTy =
				ExtractValueInst::getIndexedType(EVI.getOperand(0)->getType(), ArrayRef<unsigned int>(b,i + 1));
			if (isa < ArrayType > (IndexedTy))
				Out << ".array[" << *i << "]";
			else
				Out << ".field" << *i;
		}
	}
	//Out << ")";
}

// Use FenceInst to replace llvm.mem_barrier intrinsics
void PromelaWriter::visitFenceInst(FenceInst & FI){
	Out << "__sync_synchronize()";
}


extern "C" void
LLVMInitializePromelaBackendTarget()
{ 
  // Register the target.
  RegisterTargetMachine<PromelaTargetMachine> X(ThePromelaBackendTarget);
}

/***************************************************************************
 ***************** Main functions ******************************************
 **************************************************************************/

bool PromelaWriter::runOnModule(Module & M)
{
	TD = new DataLayout(&M);
	IL = new IntrinsicLowering(*TD);
	IL->AddPrototypes(M);

	// Ensure that all structure types have names...
	TAsm = new MCAsmInfo();
	const MCRegisterInfo *mcRegisterInfo = new MCRegisterInfo();
	const MCObjectFileInfo *mcObjectFileInfo = new MCObjectFileInfo();
	MCContext* mcc = new MCContext(*TAsm, *mcRegisterInfo, mcObjectFileInfo);
	Mang = new Mangler(*mcc, *TD);

// MM: doesn't exist anymore. Not sure what to put instead.
//	Mang->markCharUnacceptable('.');

	// Keep track of which functions are static ctors/dtors so they can have
	// an attribute added to their prototypes.
// 	std::set < Function * >StaticCtors, StaticDtors;
// 	for (Module::global_iterator I = M.global_begin(), E =
// 	     M.global_end(); I != E; ++I) {
// 		switch (getGlobalVariableClass(I)) {
// 		default:
// 			break;
// 		case GlobalCtors:
// 			FindStaticTors(I, StaticCtors);
// 			break;
// 		case GlobalDtors:
// 			FindStaticTors(I, StaticDtors);
// 			break;
// 		}
// 	}

// 	if (!M.global_empty()) {
// 		Out << "\n\n/* Global Variable Declarations */\n";
// 		for (Module::global_iterator I = M.global_begin(), E =
// 		     M.global_end(); I != E; ++I)
// 			if (!I->isDeclaration()) {
// 				// Ignore special globals, such as debug info.
// 				if (getGlobalVariableClass(I))
// 					continue;

// 				//           if (I->hasLocalLinkage())
// 				//             Out << "static ";
// 				//           else
// 				//             Out << "extern ";

// 				// Thread Local Storage
// 				if (I->isThreadLocal())
// 					Out << "__thread ";

// 				I->dump();
// // 				printType(Out,
// // 					  I->getType()->getElementType(),
// // 					  false, GetValueName(I));

// 				if (I->hasLinkOnceLinkage())
// 					Out << " __attribute__((common))";
// 				else if (I->hasCommonLinkage())	// FIXME is this right?
// 					Out << " __ATTRIBUTE_WEAK__";
// 				else if (I->hasWeakLinkage())
// 					Out << " __ATTRIBUTE_WEAK__";
// 				else if (I->hasExternalWeakLinkage())
// 					Out << " __EXTERNAL_WEAK__";
// 				if (I->hasHiddenVisibility())
// 					Out << " __HIDDEN__";
// 				Out << ";\n";
// 			}
// 	}
	/* Fill types table */
	printModuleTypes(M);
 	const Value* moduleArg;
	PointerType* PTy;	
	Function::arg_iterator argI;
	//vector < Process *> *process=this->elab->getProcesses();
	 //this->elab->getProcesses().empty();
	
	if (!( (*this->elab->getProcesses()).empty())){
	//try{
		argI = (*(this->elab->getProcesses()->begin()))->getMainFct()->arg_begin();
 		moduleArg = &*argI;		
		PTy = cast<PointerType>(moduleArg->getType());
		TRACE_4("############################# " << PTy << "  " << TypeNames[PTy->getElementType()] << "\n");
	}
	//catch (int i){ 
	else{	
		TRACE_4("############################# " << " ** No Functions or processes found ** "<< "\n");}
	/* Print Global variables from the program */
	printGlobalVariables(Mang);

	if (!( (*this->elab->getProcesses()).empty())){
		argI = (*(this->elab->getProcesses()->begin()))->getMainFct()->arg_begin();
		moduleArg = &*argI;		
		PTy = cast<PointerType>(moduleArg->getType());
		TRACE_4("***************************** " << PTy << "  " << TypeNames[PTy->getElementType()] << "\n");
	}
	else{
		TRACE_4("############################# " << " ** No Functions or processes found ** "<< "\n");
	}

	
	/* Print all stuff relative to encoding */
	printPrimitives();

	/* Print all processes and functions */
	printProcesses();
	
	/* Print init section (launch processes */
	printInitSection();

	// Free memory...
	delete IL;
	delete TD;
	delete Mang;
	delete mcRegisterInfo;
	delete mcObjectFileInfo;
	FPConstantMap.clear();
	TypeNames.clear();
	ByValParams.clear();
	intrinsicPrototypesAlreadyGenerated.clear();
	return false;

}

void PromelaWriter::getAnalysisUsage(AnalysisUsage & AU) const
{
	AU.setPreservesAll();
}

const char *PromelaWriter::getPassName() const
{
	return "Promela backend";
}

Attributes PromelaWriter::getAttributes(Attributes::AttrVal attr)
{
	return Attributes::get(getGlobalContext(),
                               ArrayRef<Attributes::AttrVal>(attr));
}

char PromelaWriter::ID = 0;
