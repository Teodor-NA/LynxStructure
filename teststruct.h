#ifndef TEST_STRUCT_H
#define TEST_STRUCT_H

#include "lynxstructure.h"

using namespace LynxLib;

struct TestStructure
{
    TestStructure(LynxManager & lynx, char _structId, const LynxString & description = "") :
		_lynxId(lynx.addStructure(_structId, description, 9)),
        i8Var(lynx, _lynxId, "i8 variable"),
        u8Var(lynx, _lynxId, "u8 variable"),
        i16Var(lynx, _lynxId, "i16 variable"),
        u16Var(lynx, _lynxId, "u16 variable"),
        i32Var(lynx, _lynxId, "i32 variable"),
		string(lynx, _lynxId, "string"),
        u32Var(lynx, _lynxId, "u32 variable"),
        i64Var(lynx, _lynxId, "i64 variable"),
        u64Var(lynx, _lynxId, "u64 variable"),
        floatVar(lynx, _lynxId, "float variable")
	{}

	// This macro must be included immediately after the constructor
	LYNX_STRUCTURE_MACRO

	LynxVar_i8 i8Var;
	LynxVar_u8 u8Var;
	LynxVar_i16 i16Var;
	LynxVar_u16 u16Var;
	LynxVar_i32 i32Var;
	LynxVar_string string;
	LynxVar_u32 u32Var;
	LynxVar_i64 i64Var;
	LynxVar_u64 u64Var;
	LynxVar_float floatVar;
};


#endif // !TEST_STRUCT_H
