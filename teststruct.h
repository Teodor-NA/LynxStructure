#ifndef TEST_STRUCT
#define TEST_STRUCT

#include "lynxstructure.h"

using namespace LynxLib;

struct TestStructure
{
    TestStructure(LynxManager & lynx, char _structId) :
		_lynxId(lynx.addStructure(_structId, 9)),
        i8Var(lynx, _lynxId),
        u8Var(lynx, _lynxId),
        i16Var(lynx, _lynxId),
        u16Var(lynx, _lynxId),
        i32Var(lynx, _lynxId),
		string(lynx, _lynxId),
        u32Var(lynx, _lynxId),
        i64Var(lynx, _lynxId),
        u64Var(lynx, _lynxId),
        floatVar(lynx, _lynxId)
	{}

	// This macro must be included immediately after the constructor
	LYNX_STRUCTURE_MACRO

//private: const LynxId _lynxId; 
//public: const LynxId & lynxId() { return _lynxId; }

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


#endif // !TEST_STRUCT
