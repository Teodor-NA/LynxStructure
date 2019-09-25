#ifndef TEST_STRUCT_H
#define TEST_STRUCT_H

#include "lynxstructure.h"

struct TestStructure
{
    TestStructure(LynxManager & lynx, char _structId, const LynxString & description = "") :
		_lynxId(lynx.addStructure(_structId, description, 9)),
        i8Var(lynx, _lynxId, "i8 var"),
        u8Var(lynx, _lynxId, "u8 var"),
        i16Var(lynx, _lynxId, "i16 var"),
        u16Var(lynx, _lynxId, "u16 var"),
        i32Var(lynx, _lynxId, "i32 var"),
        u32Var(lynx, _lynxId, "u32 var"),
        i64Var(lynx, _lynxId, "i64 var"),
        u64Var(lynx, _lynxId, "u64 var"),
        floatVar(lynx, _lynxId, "float var")
	{}

	// This macro must be included immediately after the constructor
	LYNX_STRUCTURE_MACRO

	LynxVar_i8 i8Var;
	LynxVar_u8 u8Var;
	LynxVar_i16 i16Var;
	LynxVar_u16 u16Var;
	LynxVar_i32 i32Var;
	LynxVar_u32 u32Var;
	LynxVar_i64 i64Var;
	LynxVar_u64 u64Var;
	LynxVar_float floatVar;
};


#endif // !TEST_STRUCT_H
