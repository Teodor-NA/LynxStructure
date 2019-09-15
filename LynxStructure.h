#ifndef LYNX_STRUCTURE_H
#define LYNX_STRUCTURE_H
//-----------------------------------------------------------------------------------------------------------
//-------------------------------------- LynxStructure V2.1.2.0 ---------------------------------------------
//-----------------------------------------------------------------------------------------------------------

#define LYNX_VERSION "2.1.2.0"

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

#if !defined(ARDUINO) && !defined(TI)
#define LYNX_INCLUDE_EXCEPTIONS
#endif // !ARDUINO && !TI

#define LYNX_STRUCTURE_MACRO \
private: const LynxId _lynxId; \
public: const LynxId & lynxId() { return _lynxId; }

#include <string.h>
#include <math.h>

namespace LynxLib
{
	class LynxVar;

#ifdef LYNX_INCLUDE_EXCEPTIONS
	struct LynxMessages
	{
        static const char * outOfBoundsMsg;
	};
#endif // LYNX_INCLUDE_EXCEPTIONS

	//-----------------------------------------------------------------------------------------------------------
	//-------------------------------------------- LynxList -----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	template <class T>
	class LynxList
	{
	public:
		LynxList()
		{
			_data = LYNX_NULL;
			_count = 0;
			_reservedCount = 0;
		}

		LynxList(int size) : LynxList()
		{
			this->reserve(size);
		}

		LynxList(const LynxList & other) : LynxList()
		{
			*this = other;
		}

		~LynxList()
		{
			if (_data != LYNX_NULL)
			{
				delete[] _data;
				_data = LYNX_NULL;
			}
		}

		LynxList & operator = (const LynxList & other)
		{
			if (&other == this)
				return *this;
			
			_count = 0;

			if (other._count < 1) // If the other list is empty, then copying it is pointless
				return *this;

			this->reserve(other._count);

			// memcpy(_data, other._data, other._count * sizeof(T));

			for (int i = 0; i < other._count; i++)
			{
				_data[i] = other._data[i];
			}

			_count = other._count;
			return *this;
		}

		T & operator [] (int index)
		{
#ifdef LYNX_INCLUDE_EXCEPTIONS
			if (index >= _count)
				throw LynxMessages::outOfBoundsMsg;
#endif // LYNX_INCLUDE_EXCEPTIONS

			return _data[index];
		}

		void clear() { _count = 0; }

		int count() const { return _count; }

		void reserve(int size)
		{
			_count = 0;

			if (size <= _reservedCount)
				return;

			if (_data != LYNX_NULL)
			{
				delete[] _data;
				_data = LYNX_NULL;
			}

			_reservedCount = size;
			_data = new T[_reservedCount];
		}

		void resize(int size)
		{
			if (size <= _reservedCount)
				return;

			if (_count < 1)
			{
				this->reserve(size);
				return;
			}

			T * oldData = _data;

			_reservedCount = size;

			_data = new T[_reservedCount];

			for (int i = 0; i < _count; i++)
			{
				_data[i] = oldData[i];
			}

			// memcpy(_data, oldData, _count * sizeof(T));

			delete[] oldData;
			oldData = LYNX_NULL;
		}

		int append()
		{
			this->resize(_count + 1);
			// _data[_count] = T();
			_count++;

			return (_count - 1);
		}

		int append(const T & other)
		{
			this->resize(_count + 1);
			_data[_count] = other;
			_count++;

			return (_count - 1);
		}

		int append(const LynxList<T> & other)
		{
			this->resize(other._count + _count);
			
			for (int i = 0; i < other._count; i++)
			{
				_data[_count + i] = other._data[i];
			}

			// memcpy(&_data[_count], other._data, (other._count * sizeof(T)));

			_count += other._count;

			return (_count - 1);
		}

		int append(const T * const other, int size)
		{
			this->resize(_count + size);

			for (int i = 0; i < size; i++)
			{
				_data[_count + i] = other[i];
			}

			_count += size;

			return (_count - 1);
		}

		void remove(int indexFrom, int indexTo = -1)
		{
			if (indexTo < 0)
				indexTo = indexFrom;

			if ((indexFrom < 0) || (indexFrom > indexTo) || (indexTo >= _count))
				return;

			int diff = indexTo - indexFrom + 1;
			int copySize = _count - indexTo - 1;

			for (int i = 0; i < copySize; i++)
			{
				_data[indexFrom + i] = _data[indexTo + i + 1];
			}
			// memmove(adrFrom, adrTo, copySize * sizeof(T));

			_count -= diff;
		}

		void subList(LynxList<T> & result, int startIndex, int endIndex) const
		{
			if ((startIndex < 0) || (endIndex >= _count) || (startIndex > endIndex))
				return;

			int diff = endIndex - startIndex + 1;
			result.reserve(diff);
			for (int i = 0; i < diff; i++)
			{
				result._data[i] = _data[startIndex + i];
			}

			// memcpy(result._data, _data + startIndex * sizeof(T), diff * sizeof(T));

			result._count = diff;
		}

		LynxList<T> subList(int startIndex, int endIndex) const
		{
			LynxList<T> temp;
			this->subList(temp, startIndex, endIndex);

			return temp;
		}

		const T & at(int index) const
		{
#ifdef LYNX_INCLUDE_EXCEPTIONS
			if (index >= _count)
				throw LynxMessages::outOfBoundsMsg;
#endif // LYNX_INCLUDE_EXCEPTIONS

			return _data[index];
		}

		const T & first() const { return _data[0]; }

		T & first() { return _data[0]; }

		const T & last() const { return _data[_count - 1]; }

		T & last() { return _data[_count - 1]; }

	protected:
		T * _data;
		int _count;
		int _reservedCount;
	};

	//-----------------------------------------------------------------------------------------------------------
	//----------------------------------------- LynxByteArray ---------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------


	class LynxByteArray : public LynxList<char>
	{
	public:
		LynxByteArray() : LynxList<char>() {}
		LynxByteArray(int size) : LynxList<char>(size) {}
		LynxByteArray(const char * charArray, int size);
		LynxByteArray(const LynxList<char> & other) : LynxList<char>(other) {}

		const char * data() const { return _data; }

		using LynxList::subList;
		LynxByteArray subList(int startIndex, int endIndex)
		{
			LynxByteArray temp;
			LynxList::subList(temp, startIndex, endIndex);
			return temp;
		}

		int toCharArray(char * buffer, int maxSize) const;
		int fromCharArray(const char * const buffer, int size);

	};

	//-----------------------------------------------------------------------------------------------------------
	//------------------------------------------ LynxString -----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------
	
	class LynxString
	{
	public:
		LynxString();
		LynxString(int size);
		LynxString(const char * const other, int maxLength = 255);
		LynxString(const LynxString & other);

		~LynxString();

		const char & at(int index) const;
		char & operator [] (int index);

		char & first();
		const char & first() const;
		char & last();
		const char & last() const;

		int count() const;
		void clear();

		const LynxString & operator = (const LynxString & other);
		const LynxString & operator = (const char * const other);

		void operator += (const char & other);
		void operator += (const LynxString & other);
		void operator += (const char * const other);
		
		LynxString operator + (const char & other);
		LynxString operator + (const LynxString & other);
		LynxString operator + (const char * const other);

		friend LynxString operator +(const char * const otherCharArray, const LynxString & otherString);

		operator const char * const() const;

		bool operator == (const LynxString & other);
		bool operator != (const LynxString & other);

		void resize(int size);

		LynxString subString(int startIndex, int endIndex);

		bool compare(const LynxString & other) const;
		bool isEmpty() const { return (_count < 2); }

		void append(const char & other);
		void append(const LynxString & other);
		void append(const char * const other, int maxLength = 255);

		void reverse(int indexFrom = -1, int indexTo = -1);

		static LynxString number(int64_t num, int base = 10);
		static LynxString number(uint64_t num, int base = 10);
		static LynxString number(int32_t num, int base = 10);
		static LynxString number(uint32_t num, int base = 10);

		static LynxString number(double num, int precision = 5);

		// Remove characters between indexes. Only one character will be removed if indexTo is omitted 
		void remove(int indexFrom, int indexTo = -1);
		// All characters after and including index will be removed
		void removeFrom(int index);
		// All characters before and including index will be removed
		void removeTo(int index);

		// Does not include term char
		int toByteArray(LynxByteArray & byteArray) const;
		const char * toCharArray() const;

	private:
		char * _string;
		int _count;
		int _reservedCount;

		void reserve(int size);
		static int findTermChar(const char * str, int maxLength = 255);

		static void decimalNumber(double num, LynxString & strRef, int precision);
		static void engNumber(double num, LynxString & strRef, int precision);

		// Appends num to strRef
		template <class T>
		static void numberInt(T num, LynxString & strRef, int base = 10)
		{
			if ((base < 2) || (base > 16))
				return;

			if (num == 0)
			{
				strRef += '0';
				return;
			}

			bool sign = (num < 0);
			// Absolute vlaue
			if (sign)
				num = ~num + 1;
			
			T tempNumSub = num;
			char tempNum;

			int currentCount = strRef.count();

			while (tempNumSub > 0)
			{
				tempNumSub /= base;
				tempNum = char(num - tempNumSub * base);

				if (tempNum < 10)
					strRef += tempNum + '0';
				else
					strRef += tempNum - char(10) + 'a';

				num = tempNumSub;
			}
			
			if (sign)
				strRef += '-';

			strRef.reverse(currentCount);

			
		}

	};

	//-----------------------------------------------------------------------------------------------------------
	//---------------------------------------- LynxRingBuffer ---------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	enum E_LynxRingBufferMode
	{
		eAutogrow = 0,		// Automatically grow the buffer if it is full
		eFixedOverwrite,	// Buffer has fixed size determined on construction (or with reserve()/resize()), old data is overwritten if it is full
		eFixedRefuse		// Buffer has fixed size determined on construction (or with reserve()/resize()), new data is ignored if it is full
	};

	class LynxRingBuffer
	{
	public:
		LynxRingBuffer(int size = 0, E_LynxRingBufferMode mode = eAutogrow);
		~LynxRingBuffer();

		void reserve(int size);
		void resize(int size);

		char read();
		int read(LynxByteArray & buffer, int size = -1);
		void write(char data);
		int write(const LynxByteArray & buffer, int size = -1, int startIndex = 0);
		
		int count() const { return _count; }

	private:
		char * _data;
		int _readIndex;
		int _writeIndex;
		int _reservedCount;
		int _count;
		const E_LynxRingBufferMode _mode;
	};

	//-----------------------------------------------------------------------------------------------------------
	//---------------------------------------------- Types ------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

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
		E_LynxDataType dataType;
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
	};

	class LynxType
	{
	public:
		LynxType();
		LynxType(E_LynxDataType dataType, const LynxString & description = "");
		LynxType(const LynxType & other);// : LynxType(other._dataType, other._description) { *this = other; }
		~LynxType();

		void init(E_LynxDataType dataType, const LynxString * const description);

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

		E_LynxDataType dataType() const { return _dataType; }
		int localSize() const;
		int transferSize() const;

        int toArray(LynxByteArray & buffer, E_LynxState & state) const;
        int fromArray(const LynxByteArray & buffer, int startIndex, E_LynxState & state);

		// If the program assumes the wrong endianness it can be set manually with this function
		static void setEndianness(E_Endianness endianness) { LynxType::_endianness = endianness; }

		static E_Endianness endianness() { return _endianness; }

		const LynxType & operator = (const LynxType & other) 
		{
			if (&other == this)
				return *this;

			if (_dataType == eInvalidType)
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

        E_LynxDataType _dataType;
        static E_Endianness _endianness;

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
       LynxInfo() : deviceId(0), lynxId(), dataLength(0), state(eNoChange) {}

		char deviceId;
		LynxId lynxId;
		int dataLength;
        E_LynxState state;
	};

	//-----------------------------------------------------------------------------------------------------------
	//------------------------------------- Static Functions ----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

    int splitArray(LynxByteArray & buffer, int desiredSize);

    int mergeArray(LynxByteArray & buffer, int desiredSize);

    char sizeMask(int shiftSize);

	int localSize(E_LynxDataType dataType);

	int transferSize(E_LynxDataType dataType);

	bool checkChecksum(const LynxByteArray & buffer);
	void addChecksum(LynxByteArray & buffer);

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
		E_LynxState toArray(LynxByteArray & buffer, int variableIndex = -1) const;

		/// Copies the required information to the char array
		E_LynxState toArray(char * buffer, int maxSize, int & copiedSize, int variableIndex = -1) const;

		/// Copies information from the provided buffer
        void fromArray(const LynxByteArray & buffer, LynxInfo & lynxInfo);

		/// Copies information from char array, and returns number of bytes copied
        void fromArray(const char * buffer, int size, LynxInfo & lynxInfo);

		/// Manually add a variable to the variable list
		LynxId addVariable(int structIndex, E_LynxDataType dataType, const LynxString & description = "");

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
		E_LynxState toArray(LynxByteArray & buffer, const LynxId & lynxId) const;

		// Copies the required information to the char array, and returns number of bytes copied
		E_LynxState toArray(char * buffer, int maxSize, int & copiedSize, const LynxId & lynxId) const;

		// Copies information from the provided buffer
		void fromArray(const LynxByteArray & buffer, LynxInfo & lynxInfo);

		// Copies information from char array, and returns number of bytes copied
		void fromArray(const char * buffer, int size, LynxInfo & lynxInfo);

		int transferSize(const LynxId & lynxId) const;
		int localSize(const LynxId & lynxId) const;

		LynxId addStructure(char structId, const LynxString & description = "", int size = 0);
        LynxId addVariable(const LynxId & parentStruct, E_LynxDataType dataType, const LynxString & description = "");

		// Returns number of variables in struct. Returns 0 if out of bounds
		int structVariableCount(int structIndex);

		int findId(char structId);

	private:
		char _deviceId;
		LynxType _dummyVariable;
		LynxString * _description;
	};

	//-----------------------------------------------------------------------------------------------------------
	//---------------------------------------- LynxDataTypes ----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	class LynxVar
	{
	public:
        LynxVar(LynxManager & lynxManager, const LynxId & parentStruct, E_LynxDataType dataType, const LynxString & description = "") :
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
            LynxVar(lynxManager, parentStruct, eInt8, description) {}
		
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
            LynxVar(lynxManager, parentStruct, eUint8, description) {}

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
            LynxVar(lynxManager, parentStruct, eInt16, description) {}

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
            LynxVar(lynxManager, parentStruct, eUint16, description) {}

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
            LynxVar(lynxManager, parentStruct, eInt32, description) {}

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
            LynxVar(lynxManager, parentStruct, eUint32, description) {}

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
            LynxVar(lynxManager, parentStruct, eInt64, description) {}

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
            LynxVar(lynxManager, parentStruct, eUint64, description) {}

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
            LynxVar(lynxManager, parentStruct, eFloat, description) {}

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
            LynxVar(lynxManager, parentStruct, eDouble, description) {}

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
			LynxVar(lynxManager, parentStruct, eString, description) {}

		operator const LynxString&() const { return _lynxManager->variable(_lynxId).var_string(); }

		const LynxString & operator = (const LynxString & other)
		{
			return (_lynxManager->variable(_lynxId).var_string() = other);
		}
	};

	// extern LynxManager Lynx;
}
#endif // !LYNX_STRUCTURE_H
