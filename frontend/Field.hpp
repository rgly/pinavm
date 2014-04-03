#ifndef _FIELD_HPP
#define _FIELD_HPP

using namespace llvm;

class Field {
      private:
	string moduleName, className, fieldName;

      public:
	Field(string moduleName, string className, string fieldName);
};

#endif
