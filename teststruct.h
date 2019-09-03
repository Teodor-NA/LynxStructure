#ifndef TEST_STRUCT
#define TEST_STRUCT

#include "lynxstructure.h"

using namespace LynxLib;

struct TestStructure
{
    TestStructure(LynxManager & lynx, char _structId) :
		lynxId(lynx.addStructure(_structId, 9)),
		i8Var(lynx.addVariable(lynxId, eInt8)),
		u8Var(lynx.addVariable(lynxId, eUint8)),
		i16Var(lynx.addVariable(lynxId, eInt16)),
		u16Var(lynx.addVariable(lynxId, eUint16)),
		i32Var(lynx.addVariable(lynxId, eInt32)),
		u32Var(lynx.addVariable(lynxId, eUint32)),
		i64Var(lynx.addVariable(lynxId, eInt64)),
		u64Var(lynx.addVariable(lynxId, eUint64)),
		floatVar(lynx.addVariable(lynxId, eFloat))
		// doubleVar(lynx.addVariable(lynxId, eDouble))
	{}

	const LynxId lynxId;
	LynxVar_i8 i8Var;
	LynxVar_u8 u8Var;
	LynxVar_i16 i16Var;
	LynxVar_u16 u16Var;
	LynxVar_i32 i32Var;
	LynxVar_u32 u32Var;
	LynxVar_i64 i64Var;
	LynxVar_u64 u64Var;
	LynxVar_float floatVar;
	// LynxVar_double doubleVar;
};


#endif // !TEST_STRUCT
