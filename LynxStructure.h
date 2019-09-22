#ifndef LYNX_STRUCTURE_H
#define LYNX_STRUCTURE_H
//-----------------------------------------------------------------------------------------------------------
//-------------------------------------- LynxStructure V2.1.2.1 ---------------------------------------------
//-----------------------------------------------------------------------------------------------------------

#define LYNX_VERSION "2.1.2.1"

#ifdef TI
typedef uint16_t uint8_t
typedef int16_t int8_t
#else
#include <stdint.h>
#endif // TI

#ifndef LYNX_NULL
#ifdef TI
#define LYNX_NULL 0
#else
#define LYNX_NULL nullptr
#endif // TI
#endif // !LYNX_NULL

#define SIZE_64 sizeof(int64_t) // The local size of a 64 bit integer

#define LYNX_STATIC_HEADER 'A'	// Static header for Lynx datagrams (always the first byte of a datagram)
#define LYNX_HEADER_BYTES 5		// Number of header bytes
#define LYNX_CHECKSUM_BYTES 1	// Number of checksum bytes

#define LYNX_INTERNALS_HEADER char(255)

#define LYNX_STRUCTURE_MACRO \
private: const LynxId _lynxId; \
public: const LynxId & lynxId() { return _lynxId; }

#include "lynxlistclasses.h"

//-----------------------------------------------------------------------------------------------------------
//----------------------------- Pre-declaration of Structs and Classes --------------------------------------
//-----------------------------------------------------------------------------------------------------------

union LynxUnion;

struct LynxVariableInfo;
struct LynxStructInfo;
struct LynxDeviceInfo;

struct LynxId;
struct LynxInfo;

class LynxType;
class LynxVar;

class LynxStructure;
class LynxManager;

class LynxVar;
class LynxVar_i8;
class LynxVar_u8;
class LynxVar_i16;
class LynxVar_u16;
class LynxVar_i32;
class LynxVar_u32;
class LynxVar_i64;
class LynxVar_u64;
class LynxVar_float;
class LynxVar_double;
class LynxVar_string;

//-----------------------------------------------------------------------------------------------------------
//--------------------------------------- Enums and static functions ----------------------------------------
//-----------------------------------------------------------------------------------------------------------

namespace LynxLib
{ 
	enum E_LynxInternals
	{
		eInvalidInternal = 0,
		eDeviceInfo,
		eScan
	};

	enum E_LynxState
	{
		eNoChange = 0,
		eNewDataReceived,
		eNewDeviceInfoReceived,
		eScanReceived,
		eDataCopiedToBuffer,
		eOutOfSync,
		eStructIdNotFound,
		eVariableIndexOutOfBounds,
		eBufferTooSmall,
		eWrongChecksum,
		eWrongStaticHeader,
		eWrongDataLength,
		eDataLengthNotFound,
		eNoStructuresInList,
		eStructIndexOutOfBounds,
		eSplitArrayFailed,
		eMergeArrayFailed,
		eEndiannessNotSet,
		eUnknownError,
		eLynxState_EndOfList
	};
	extern const LynxString lynxStateTextList[E_LynxState::eLynxState_EndOfList]; 

	enum E_LynxDataType
	{
		eInvalidType = 0,
		eInt8,
		eUint8,
		eInt16,
		eUint16,
		eInt32,
		eUint32,
		eInt64,
		eUint64,
		eFloat,
		eDouble,
		eString,
		eLynxType_EndOfList
	};
	extern const LynxString lynxTypeTextList[E_LynxDataType::eLynxType_EndOfList];

	enum E_Endianness
	{
		eNotSet = 0,
		eBigEndian,
		eLittleEndian
	};

	int splitArray(LynxByteArray & buffer, int desiredSize);

	int mergeArray(LynxByteArray & buffer, int desiredSize);

	char sizeMask(int shiftSize);

	int localSize(LynxLib::E_LynxDataType dataType);

	int transferSize(LynxLib::E_LynxDataType dataType);

	bool checkChecksum(const LynxByteArray & buffer);
	void addChecksum(LynxByteArray & buffer);
}

//-----------------------------------------------------------------------------------------------------------
//------------------------------------------- Structures ----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

union LynxUnion
{
	char bytes[SIZE_64];
	int8_t _var_i8;
	uint8_t _var_u8;
	int16_t _var_i16;
	uint16_t _var_u16;
	int32_t _var_i32;
	uint32_t _var_u32;
	float _var_float;
	int64_t _var_i64;
	uint64_t _var_u64;
	double _var_double;
};

struct LynxVariableInfo
{
	char index;
	LynxLib::E_LynxDataType dataType;
	LynxString description;
};

struct LynxStructInfo
{
	char structId;
	int variableCount;
	LynxString description;
	LynxList<LynxVariableInfo> variables;
};

struct LynxDeviceInfo
{
	char deviceId;
	int structCount;
	LynxString lynxVersion;
	LynxString description;
	LynxList<LynxStructInfo> structs;

	void deleteInfo()
	{
		deviceId = 0;
		structCount = 0;
		lynxVersion.deleteData();
		description.deleteData();
		structs.deleteData();
	}
};

struct LynxId
{
	// LynxId() : structIndex(-1), variableIndex(-1) {}
	LynxId(int _structIndex = -1, int _variableIndex = -1) : structIndex(_structIndex), variableIndex(_variableIndex) {}

	bool operator == (const LynxId & other) const
	{
		return ((structIndex == other.structIndex) && (variableIndex == other.variableIndex));
	}

	bool operator != (const LynxId & other) const
	{
		return ((structIndex != other.structIndex) || (variableIndex != other.variableIndex));
	}


	int structIndex;
	int variableIndex;
};

struct LynxInfo
{
	LynxInfo() : deviceId(0), lynxId(), dataLength(0), state(LynxLib::eNoChange) {}

	char deviceId;
	LynxId lynxId;
	int dataLength;
	LynxLib::E_LynxState state;
};

//-----------------------------------------------------------------------------------------------------------
//------------------------------------------- LynxType ------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

class LynxType
{
public:
	LynxType();
	LynxType(LynxLib::E_LynxDataType dataType, const LynxString & description = "");
	LynxType(const LynxType & other);// : LynxType(other._dataType, other._description) { *this = other; }
	~LynxType();

	void init(LynxLib::E_LynxDataType dataType, const LynxString * const description);

	LynxString description();
	// void getInfo(LynxVariableInfo & variableInfo) const;
	// LynxVariableInfo getInfo() const;

    int8_t & var_i8() { return _var->_var_i8; }
    uint8_t & var_u8() { return _var->_var_u8; }
    int16_t & var_i16() { return _var->_var_i16; }
    uint16_t & var_u16() { return _var->_var_u16; }
    int32_t & var_i32() { return _var->_var_i32; }
    uint32_t & var_u32() { return _var->_var_u32; }
    float & var_float() { return _var->_var_float; }
    int64_t & var_i64() { return _var->_var_i64; }
    uint64_t & var_u64() { return _var->_var_u64; }
    double & var_double() { return _var->_var_double; }
	LynxString & var_string() { return *_str; }

    const int8_t & var_i8() const { return _var->_var_i8; }
    const uint8_t & var_u8() const { return _var->_var_u8; }
    const int16_t & var_i16() const { return _var->_var_i16; }
    const uint16_t & var_u16() const { return _var->_var_u16; }
    const int32_t & var_i32() const { return _var->_var_i32; }
    const uint32_t & var_u32() const { return _var->_var_u32; }
    const float & var_float() const { return _var->_var_float; }
    const int64_t & var_i64() const { return _var->_var_i64; }
    const uint64_t & var_u64() const { return _var->_var_u64; }
    const double & var_double() const { return _var->_var_double; }
	const LynxString & var_string() const { return *_str; }

	LynxLib::E_LynxDataType dataType() const { return _dataType; }
	int localSize() const;
	int transferSize() const;

    int toArray(LynxByteArray & buffer, LynxLib::E_LynxState & state) const;
    int fromArray(const LynxByteArray & buffer, int startIndex, LynxLib::E_LynxState & state);

	// If the program assumes the wrong endianness it can be set manually with this function
	static void setEndianness(LynxLib::E_Endianness endianness) { LynxType::_endianness = endianness; }

	static LynxLib::E_Endianness endianness() { return _endianness; }

	const LynxType & operator = (const LynxType & other) 
	{
		if (&other == this)
			return *this;

		if (_dataType == LynxLib::eInvalidType)
			this->init(other._dataType, other._description);

		if ((_var != LYNX_NULL) && (other._var != LYNX_NULL))
			_var->_var_i64 = other._var->_var_i64;

		if ((_str != LYNX_NULL) && (other._str != LYNX_NULL))
			*_str = *(other._str);

		// _dataType = other._dataType;

		return *this;
    }

private:
	LynxUnion * _var;
	LynxString * _str;

	LynxString * _description; // optional

	LynxLib::E_LynxDataType _dataType;
    static LynxLib::E_Endianness _endianness;

};

//-----------------------------------------------------------------------------------------------------------
//---------------------------------------- LynxStructure ----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

class LynxStructure : LynxList<LynxType>
{
public:
	LynxStructure();
	~LynxStructure();

	const LynxStructure & operator = (const LynxStructure & other)
	{
		this->init(other._structId, other._description, other._count);

		LynxList::operator=(other);
			
		return *this;
	}

	using LynxList::operator[];
	using LynxList::at;
	using LynxList::count;
	using LynxList::reserve;

	void init(char structId, const LynxString * const description, int size = 0);

	void getInfo(LynxStructInfo & structInfo) const;
	LynxStructInfo getInfo() const;

	/// Copies the desired information to the provided buffer
	LynxLib::E_LynxState toArray(LynxByteArray & buffer, int variableIndex = -1) const;

	/// Copies the required information to the char array
	LynxLib::E_LynxState toArray(char * buffer, int maxSize, int & copiedSize, int variableIndex = -1) const;

	/// Copies information from the provided buffer
    void fromArray(const LynxByteArray & buffer, LynxInfo & lynxInfo);

	/// Copies information from char array, and returns number of bytes copied
    void fromArray(const char * buffer, int size, LynxInfo & lynxInfo);

	/// Manually add a variable to the variable list
	LynxId addVariable(int structIndex, LynxLib::E_LynxDataType dataType, const LynxString & description = "");

	/// Returns the transfersize of requested data (not including header and checksum)
	int transferSize(int variableIndex = -1) const;

	/// Returns the local size of requested data (not including header and checksum)
	int localSize(int variableIndex = -1) const;

    // void setStructId(char structId) { _structId = structId; }
	char structId() const { return _structId; }

private:
	char _structId;
	LynxString * _description;
	// int _localSize;
	// int _transferSize;
};

//-----------------------------------------------------------------------------------------------------------
//------------------------------------------ LynxManager ----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

class LynxManager : private LynxList<LynxStructure>
{
public:
	LynxManager(char deviceId = char(0xff), const LynxString & description = "", int size = 0);
	~LynxManager();

	using LynxList::count;

	void getInfo(LynxDeviceInfo & deviceInfo) const;
	LynxDeviceInfo getInfo() const;

	void setDeviceId(char deviceId) { _deviceId = deviceId; }
    char structId(const LynxId & lynxId);

	LynxType & variable(const LynxId & lynxId);
	const LynxType & variable(const LynxId & lynxId) const;

	void copy(const LynxId & source, const LynxId & target);

	// Creates a buffer with the desired information, and returns it
	// LynxByteArray toArray(const LynxId & lynxId) const;

	// Copies the desired information to the provided buffer
	LynxLib::E_LynxState toArray(LynxByteArray & buffer, const LynxId & lynxId) const;

	// Copies the required information to the char array, and returns number of bytes copied
	LynxLib::E_LynxState toArray(char * buffer, int maxSize, int & copiedSize, const LynxId & lynxId) const;

	// Copies information from the provided buffer
	void fromArray(const LynxByteArray & buffer, LynxInfo & lynxInfo);

	// Copies information from char array, and returns number of bytes copied
	void fromArray(const char * buffer, int size, LynxInfo & lynxInfo);

	int transferSize(const LynxId & lynxId) const;
	int localSize(const LynxId & lynxId) const;

	LynxId addStructure(char structId, const LynxString & description = "", int size = 0);
    LynxId addVariable(const LynxId & parentStruct, LynxLib::E_LynxDataType dataType, const LynxString & description = "");

	// Returns number of variables in struct. Returns 0 if out of bounds
	int structVariableCount(int structIndex);

	int findId(char structId);

private:
	char _deviceId;
	LynxType _dummyVariable;
	LynxString * _description;
};

//-----------------------------------------------------------------------------------------------------------
//------------------------------------ LynxVar and variants -------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

class LynxVar
{
public:
    LynxVar(LynxManager & lynxManager, const LynxId & parentStruct, LynxLib::E_LynxDataType dataType, const LynxString & description = "") :
        _lynxManager(&lynxManager),
        _lynxId(lynxManager.addVariable(parentStruct, dataType, description))
    {}

    LynxVar(const LynxVar & other) :
        _lynxManager(other._lynxManager),
        _lynxId(other._lynxId)

    { *this = other; }

    const LynxId & lynxId() const { return _lynxId; }
	const LynxVar & operator = (const LynxVar & other)
	{
        if(&other == this)
            return *this;

		_lynxManager->copy(other._lynxId, this->_lynxId);
		return *this;
	}

protected:
    LynxManager * const _lynxManager;
    const LynxId _lynxId;
};

class LynxVar_i8 : public LynxVar
{
public:
    // LynxVar_i8(const LynxVar & other) : LynxVar(other) {}
    LynxVar_i8(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eInt8, description) {}
		
	operator const int8_t&() const { return _lynxManager->variable(_lynxId).var_i8(); }

	const int8_t & operator = (const int8_t & other) 
	{
		return (_lynxManager->variable(_lynxId).var_i8() = other);
	}
};


class LynxVar_u8 : public LynxVar
{
public:
    // LynxVar_u8(const LynxVar & other) : LynxVar(other) {}
    LynxVar_u8(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eUint8, description) {}

	operator const uint8_t&() const { return _lynxManager->variable(_lynxId).var_u8(); }

	const uint8_t & operator = (const uint8_t & other)
	{
		return (_lynxManager->variable(_lynxId).var_u8() = other);
	}
}; 

class LynxVar_i16 : public LynxVar
{
public:
    // LynxVar_i16(const LynxVar & other) : LynxVar(other) {}
    LynxVar_i16(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eInt16, description) {}

	operator const int16_t&() const { return _lynxManager->variable(_lynxId).var_i16(); }

	const int16_t & operator = (const int16_t & other)
	{
		return (_lynxManager->variable(_lynxId).var_i16() = other);
	}
};

class LynxVar_u16 : public LynxVar
{
public:
    // LynxVar_u16(const LynxVar & other) : LynxVar(other) {}
    LynxVar_u16(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eUint16, description) {}

	operator const uint16_t&() const { return _lynxManager->variable(_lynxId).var_u16(); }

	const uint16_t & operator = (const uint16_t & other)
	{
		return (_lynxManager->variable(_lynxId).var_u16() = other);
	}
};

class LynxVar_i32 : public LynxVar
{
public:
    // LynxVar_i32(const LynxVar & other) : LynxVar(other) {}
    LynxVar_i32(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eInt32, description) {}

	operator const int32_t&() const { return _lynxManager->variable(_lynxId).var_i32(); }

	const int32_t & operator = (const int32_t & other)
	{
		return (_lynxManager->variable(_lynxId).var_i32() = other);
	}
};

class LynxVar_u32 : public LynxVar
{
public:
    // LynxVar_u32(const LynxVar & other) : LynxVar(other) {}
    LynxVar_u32(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eUint32, description) {}

	operator const uint32_t&() const { return _lynxManager->variable(_lynxId).var_u32(); }

	const uint32_t & operator = (const uint32_t & other)
	{
		return (_lynxManager->variable(_lynxId).var_u32() = other);
	}
};

class LynxVar_i64 : public LynxVar
{
public:
    // LynxVar_i64(const LynxVar & other) : LynxVar(other) {}
    LynxVar_i64(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eInt64, description) {}

	operator const int64_t&() const { return _lynxManager->variable(_lynxId).var_i64(); }

	const int64_t & operator = (const int64_t & other)
	{
		return (_lynxManager->variable(_lynxId).var_i64() = other);
	}
};

class LynxVar_u64 : public LynxVar
{
public:
    // LynxVar_u64(const LynxVar & other) : LynxVar(other) {}
    LynxVar_u64(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eUint64, description) {}

	operator const uint64_t&() const { return _lynxManager->variable(_lynxId).var_u64(); }

	const uint64_t & operator = (const uint64_t & other)
	{
		return (_lynxManager->variable(_lynxId).var_u64() = other);
	}
};

class LynxVar_float : public LynxVar
{
public:
    // LynxVar_float(const LynxVar & other) : LynxVar(other) {}
    LynxVar_float(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eFloat, description) {}

	operator const float&() const { return _lynxManager->variable(_lynxId).var_float(); }

	const float & operator = (const float & other)
	{
		return (_lynxManager->variable(_lynxId).var_float() = other);
	}
};

class LynxVar_double : public LynxVar
{
public:
    // LynxVar_double(const LynxVar & other) : LynxVar(other) {}
    LynxVar_double(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
        LynxVar(lynxManager, parentStruct, LynxLib::eDouble, description) {}

	operator const double&() const { return _lynxManager->variable(_lynxId).var_double(); }

	const double & operator = (const double & other)
	{
		return (_lynxManager->variable(_lynxId).var_double() = other);
	}
};

class LynxVar_string : public LynxVar
{
public:
	// LynxVar_double(const LynxVar & other) : LynxVar(other) {}
	LynxVar_string(LynxManager & lynxManager, const LynxId & parentStruct, const LynxString & description = "") :
		LynxVar(lynxManager, parentStruct, LynxLib::eString, description) {}

	operator const LynxString&() const { return _lynxManager->variable(_lynxId).var_string(); }

	const LynxString & operator = (const LynxString & other)
	{
		return (_lynxManager->variable(_lynxId).var_string() = other);
	}
};

// extern LynxManager Lynx;

#endif // !LYNX_STRUCTURE_H
